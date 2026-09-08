# APM32M ADS1299 Ready-to-Run Guide

This file is additive. Existing APM32M source, port, board, example and test files remain unchanged.

## Bring-up

1. Configure the selected SPI peripheral as ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable driver.
4. Read and validate the device ID.
5. Validate the complete digital path with the ADS1299 internal test source.
6. Validate the PGA/ADC noise path using the input-short profile.
7. Start normal acquisition at 250 SPS.

## Frame handling

An ADS1299-8 RDATAC frame contains 27 bytes. Read exactly one full frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI transfer -> status/header check -> fixed ring buffer -> transport`

The ISR must remain short and deterministic; transport formatting belongs outside it.

## APM32M notes

APM32M covers multiple motor/control-oriented devices, so keep board/peripheral selection in the model-specific layer rather than hard-coding one pin map here. Use the blocking path for first validation, then add DMA where the chosen APM32M device and SDK provide a suitable SPI/DMA route.

For 8 x ADS1299 / 64 channels, use this family for control/transport or multi-lane acquisition when resources are sufficient. For strict synchronization, use the FPGA capture layer as the deterministic front end.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> higher rate if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/geehy_ads1299_ready_app.[ch]`.
