# APM32F072 ADS1299 Ready-to-Run Guide

This file is additive. Existing APM32F072 port, board, example and test files remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run the internal-test profile and then the input-short profile.
6. Start normal EEG acquisition at 250 SPS.

## Frame handling

For ADS1299-8, each RDATAC frame is exactly 27 bytes: 3 status bytes plus 8 x 24-bit channel samples.

Recommended path:

`DRDY -> acquisition event -> 27-byte SPI transfer -> frame validation -> fixed ring buffer -> USB/UART transport`

Keep the ISR short. Do not print, packetize or convert to floating point inside the interrupt.

## APM32F072 notes

APM32F072 is appropriate for a compact single-ADS1299 node at conservative rates. Keep buffers fixed-size and memory use deterministic. Validate a blocking SPI path first; introduce DMA only after the internal-test waveform is stable.

For 8 x ADS1299 / 64-channel systems, use this device mainly as a control/interface MCU or place deterministic multi-device capture in the repository FPGA layer.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`

See `../DIRECT_USE.md` and `../_shared/geehy_ads1299_ready_app.[ch]`.
