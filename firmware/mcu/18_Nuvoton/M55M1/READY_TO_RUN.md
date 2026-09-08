# M55M1 ADS1299 Ready-to-Run Guide

This file is additive. Existing M55M1 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run the internal-test profile and then the input-short profile.
6. Start normal EEG acquisition at 250 SPS before higher rates are enabled.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> timestamp/sequence -> high-priority acquisition event -> SPI/DMA 27-byte read -> status/header validation -> ring buffer -> transport/processing`

## M55M1 notes

M55M1 has ample compute for acquisition plus local signal processing, but real-time capture should still be isolated from DSP/AI workloads. First validate the blocking SPI path using the internal test source, then use DMA and bounded ping-pong/ring buffers. If cache is enabled, ensure DMA buffers are placed and maintained with correct cache-coherency rules for the selected SDK/memory region.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes where possible. For the tightest inter-device timing, use the repository FPGA capture layer and let M55M1 perform control, buffering, DSP and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> DMA/cache stress -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/nuvoton_ads1299_ready_app.[ch]`.
