# CH32V203 ADS1299 Ready-to-Run Guide

This file is additive. Keep the existing `ads1299_port/`, board files, examples, tests, and documentation unchanged.

## Recommended bring-up

1. Configure hardware SPI as ADS1299 SPI mode 1 (CPOL=0, CPHA=1). Start conservatively around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as GPIO outputs. Configure DRDY as a falling-edge external interrupt.
3. Bind the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the ADS1299 ID register before enabling normal acquisition.
5. Run the internal test-signal profile, then the input-short profile, before connecting electrodes.
6. Start normal EEG acquisition at 250 SPS first. Each ADS1299-8 RDATAC frame is 27 bytes: 3 status bytes plus 8 x 24-bit channel samples.

## Acquisition path

Keep the DRDY ISR short. Set a flag or release a task/event and perform the 27-byte SPI transfer outside the ISR. Push completed frames into a fixed-size ring buffer before USB/UART processing.

Suggested flow:

`DRDY falling edge -> high-priority acquisition service -> 27-byte SPI read -> frame validation -> ring buffer -> transport`

Track at least sample sequence, read errors and buffer-overflow count.

## CH32V203 notes

CH32V203 is suitable for a single ADS1299 at conservative sample rates when the acquisition path remains deterministic. Prefer DMA for sustained transfers when the selected WCH peripheral library supports it, but first prove the blocking 27-byte path with the internal test signal.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes or place deterministic capture in the FPGA layer already provided by this repository. The MCU can then handle control and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/wch_ads1299_ready_app.[ch]` for the common application flow.
