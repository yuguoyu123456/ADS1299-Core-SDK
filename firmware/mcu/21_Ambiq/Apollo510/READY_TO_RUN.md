# Apollo510 ADS1299 Ready-to-Run Guide

This file is additive. Existing Apollo510 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure IOM/SPI for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before higher rates are enabled.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> timestamp/sequence -> high-priority acquisition event -> IOM/SPI DMA read -> frame validation -> bounded ring buffer -> DSP/AI/transport`

## Apollo510 notes

Apollo510 can combine acquisition with substantial local processing, but acquisition must remain isolated from DSP/AI and display workloads. First validate a blocking transfer with the internal test source, then use DMA/non-blocking transfers and bounded buffers. If caches or heterogeneous memory regions are enabled, place DMA buffers in compatible memory and apply the SDK's coherency rules.

For 8 x ADS1299 / 64 channels, multiple capture lanes can be supervised by Apollo510 when resources allow. For strict low-skew synchronization, use the repository FPGA capture layer as the deterministic front end and use Apollo510 for control, buffering, DSP and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> DMA/cache stress -> DSP-load stress -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/ambiq_ads1299_ready_app.[ch]`.
