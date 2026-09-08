# APM32F407 ADS1299 Ready-to-Run Guide

This file is additive. Existing APM32F407 source, port, board, examples and tests are preserved.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as GPIO outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG at 250 SPS before enabling higher rates.

## Continuous acquisition

For ADS1299-8, one RDATAC frame is 27 bytes: 3 status bytes plus 8 x 24-bit channel data.

Recommended architecture:

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> frame validation -> ring buffer -> transport`

Keep USB/UART/Ethernet processing and floating-point conversion outside the timing-critical path.

## APM32F407 notes

APM32F407 is a good fit for sustained ADS1299 acquisition. First prove the blocking path using the internal test source, then use SPI DMA with ping-pong or ring buffering. Track SPI errors, sequence gaps, late servicing and queue overflow explicitly.

For 8 x ADS1299 / 64-channel systems, multiple SPI/DMA lanes can be supervised by this class of MCU, but the FPGA capture layer remains the preferred front end when tight inter-device skew is required.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/geehy_ads1299_ready_app.[ch]`.
