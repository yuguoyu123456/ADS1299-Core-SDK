# AT32F421 ADS1299 Ready-to-Run Guide

This file is additive. Existing AT32F421 source, board, port, example and test files remain unchanged.

## Bring-up

1. Configure SPI master as ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks.
4. Initialize the ADS1299 core, read the ID and verify the expected device family/channel count.
5. Validate the digital path with the internal test signal.
6. Validate the analog/ADC path with the input-short profile.
7. Start normal EEG acquisition at 250 SPS.

## Frame handling

One ADS1299-8 RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event and enqueue the frame before any transport formatting.

`DRDY -> acquisition signal -> 27-byte SPI transfer -> frame check -> ring buffer -> USB/UART`

Do not perform printf, floating-point conversion or long packetization inside the interrupt.

## AT32F421 notes

AT32F421 is better suited to compact single-ADS1299 nodes than to direct 64-channel aggregation. Keep memory usage deterministic, use fixed-size frame buffers and first validate a blocking transfer before adding DMA.

For 8 x ADS1299 / 64-channel systems, treat AT32F421 as a control/interface MCU or use an FPGA/more capable controller for deterministic multi-device capture.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`

See `../DIRECT_USE.md` and `../_shared/artery_ads1299_ready_app.[ch]`.
