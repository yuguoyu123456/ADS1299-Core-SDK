# RX660 + ADS1299 Ready-to-Run Guide

This file adds practical integration guidance only. Existing RX660 files remain unchanged.

## RX integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to RSPI/GPIO/external interrupt resources configured through FIT/Smart Configurator or the project’s register-level driver, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- RSPI/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for hardware bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition architecture

The DRDY ISR should only acknowledge the interrupt and notify a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and places the complete frame into a ring buffer.

UART/USB/Ethernet/logging and signal processing should be downstream consumers.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform long continuous capture and monitor frame alignment/read errors/buffer overflow.
7. Add BIAS, lead-off and faster rates after baseline stability.

## DMA note

After a reliable interrupt/polling baseline is established, DTC/DMAC or RSPI transfer support can reduce CPU load. Use explicit ping-pong/ring-buffer ownership between the transfer path and consumers.

## 64-channel note

For eight ADS1299 devices, prefer multiple SPI/DMA lanes or an FPGA capture stage when tight synchronization matters. RX660 can then handle configuration, aggregation and transport.

See `../DIRECT_USE.md` for the common Renesas flow.