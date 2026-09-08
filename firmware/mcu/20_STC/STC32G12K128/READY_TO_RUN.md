# STC32G12K128 ADS1299 Ready-to-Run Guide

This file is additive. Existing STC32G12K128 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event and enqueue it before transport processing.

`DRDY -> acquisition event -> 27-byte SPI read -> status/header validation -> fixed ring buffer -> transport`

## STC32G12K128 notes

Keep acquisition simple and deterministic. Use fixed-size buffers, keep logging and floating-point conversion outside the ISR, and transport raw 24-bit data when possible. Validate a blocking SPI path first; add DMA only if the selected STC32 configuration provides a suitable route.

For 8 x ADS1299 / 64-channel systems, use this MCU mainly as a control/interface controller or pair it with the repository FPGA capture layer for deterministic multi-device acquisition.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`.

See `../DIRECT_USE.md` and `../_shared/stc_ads1299_ready_app.[ch]`.
