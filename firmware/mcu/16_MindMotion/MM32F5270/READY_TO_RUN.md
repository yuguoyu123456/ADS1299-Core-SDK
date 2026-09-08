# MM32F5270 ADS1299 Ready-to-Run Guide

This file is additive. Existing MM32F5270 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before raising the sample rate.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> frame validation -> ring buffer -> transport`

## MM32F5270 notes

MM32F5270 is well suited to sustained ADS1299 acquisition. After blocking-path validation, use SPI DMA with ping-pong or ring buffering. Keep transport and floating-point conversion outside the timing-critical acquisition path, and track SPI errors, sequence gaps, late service and queue overflows.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes if available. For strict synchronization, prefer `firmware/fpga/common` for deterministic capture and use MM32F5270 for control and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/mindmotion_ads1299_ready_app.[ch]`.
