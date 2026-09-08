# RX671 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing RX671 files remain unchanged.

## RX integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to RSPI/GPIO/external interrupt resources configured through FIT/Smart Configurator or the existing project driver, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- RSPI/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for initial board validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Real-time acquisition

Use the DRDY ISR only to notify a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates counters and places the complete frame into a ring buffer before any signal processing or transport begins.

Recommended flow:

`DRDY -> IRQ notification -> 27-byte RSPI transfer -> ring buffer -> USB/Ethernet/UART/DSP/application`

## Validation order

1. Hardware reset and ADS1299 ID verification.
2. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
4. `RENESAS_ADS1299_PROFILE_EEG_250`.
5. Perform long captures under final application load.
6. Monitor frame alignment, read errors and buffer overflow.
7. Add BIAS, lead-off and higher sample rates only after stable baseline operation.

## DMA/DTC note

After proving the basic RSPI path, use DTC/DMAC if needed to reduce CPU load. Keep buffer ownership explicit and retain a simple non-DMA path for debugging frame-corruption issues.

## 64-channel note

For 8 x ADS1299, multiple independent SPI/DMA lanes or an FPGA front-end are preferred when tight inter-device timing is required. RX671 can then focus on aggregation, DSP, logging and communication.

See `../DIRECT_USE.md` for the common Renesas integration flow.