# HC32F460 ADS1299 Ready-to-Run Guide

This file is additive. Existing HC32F460 source, board, port, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as GPIO outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable driver.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG streaming at 250 SPS before increasing the sample rate.

## Continuous acquisition

An ADS1299-8 RDATAC frame is 27 bytes. Service exactly one full frame for every DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte read -> status/header check -> ring buffer -> transport`

Keep transport and floating-point conversion out of the DRDY ISR.

## HC32F460 notes

HC32F460 is a strong fit for sustained ADS1299 acquisition. After blocking-path validation with the internal test source, use SPI DMA and ping-pong/ring buffers. Keep counters for SPI errors, late servicing, sequence gaps and queue overflows.

For 8 x ADS1299 / 64 channels, multiple SPI/DMA lanes are preferred. If low inter-device skew is a hard requirement, use the FPGA capture layer in `firmware/fpga/common` and let HC32F460 handle configuration and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/hdsc_ads1299_ready_app.[ch]`.
