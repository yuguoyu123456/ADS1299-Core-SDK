# MM32F3277 ADS1299 Ready-to-Run Guide

This file is additive. Existing MM32F3277 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks.
4. Initialize the ADS1299 core and validate the device ID.
5. Run the internal-test profile and then the input-short profile.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Keep the DRDY ISR short and service one complete frame per event.

`DRDY -> acquisition event -> SPI/DMA 27-byte read -> status/header check -> ring buffer -> transport`

## MM32F3277 notes

MM32F3277 is a practical single-ADS1299 controller. First validate blocking SPI with the internal test signal; then move to DMA and ping-pong/ring buffering if required. Track read errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes where practical or use the repository FPGA capture layer for low-skew deterministic acquisition.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device sync`.

See `../DIRECT_USE.md` and `../_shared/mindmotion_ads1299_ready_app.[ch]`.
