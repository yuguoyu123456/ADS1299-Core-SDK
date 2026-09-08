# APM32F103 ADS1299 Ready-to-Run Guide

This file is additive. Existing APM32F103 source, board, port, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master as ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks.
4. Initialize the portable core and verify the ADS1299 ID register.
5. Run the internal calibration test signal.
6. Run the input-short profile.
7. Start normal EEG at 250 SPS before increasing the sample rate.

## Acquisition path

One ADS1299-8 RDATAC frame is 27 bytes. Service one complete frame per DRDY event.

`DRDY -> high-priority acquisition event -> 27-byte SPI read -> status/header validation -> ring buffer -> transport`

Do not perform printf or transport packet construction inside the interrupt.

## APM32F103 notes

APM32F103 is a practical single-ADS1299 controller. Use fixed-size buffers and explicit error counters. After the blocking transfer has been proven with the internal test source, SPI DMA can be introduced if the selected SDK/peripheral mapping supports it.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes where feasible or offload deterministic capture to `firmware/fpga/common`.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/geehy_ads1299_ready_app.[ch]`.
