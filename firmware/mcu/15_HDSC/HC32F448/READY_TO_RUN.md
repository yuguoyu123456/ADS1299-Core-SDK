# HC32F448 ADS1299 Ready-to-Run Guide

This file is additive. Existing HC32F448 source, port, board, example and test files remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

## Frame handling

For ADS1299-8, one RDATAC frame is exactly 27 bytes: 3 status bytes plus 8 x 24-bit channel values.

Recommended path:

`DRDY -> acquisition event -> 27-byte SPI transfer -> header/status validation -> ring buffer -> transport`

Keep the DRDY ISR short. Do not perform printf, packetization or floating-point conversion inside it.

## HC32F448 notes

HC32F448 is suitable for a deterministic single-ADS1299 node and can use DMA after the blocking path is validated. Prefer fixed-size buffers and track read errors, sequence gaps, late service and queue overflow explicitly.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes when resources allow or use the repository FPGA capture layer for strict inter-device timing.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/hdsc_ads1299_ready_app.[ch]`.
