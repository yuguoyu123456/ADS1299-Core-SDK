# SAMG55 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing SAMG55 files remain unchanged.

## Harmony / ASF-style integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to the project SPI/GPIO/external interrupt implementation, then add `../_shared/microchip_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz during hardware bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge interrupt input.

## Acquisition flow

Keep the DRDY ISR minimal and use it only to notify the acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates counters and writes the complete frame to a ring buffer.

USB/UART/networking/logging should consume buffered frames asynchronously and must not block sampling.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture and monitor frame alignment/read errors/overflow.
7. Add BIAS, lead-off and faster rates only after stable baseline capture.

## DMA note

After a reliable non-DMA SPI path is established, use DMA/PDC-style transfer support where appropriate with explicit ping-pong buffer ownership.

## 64-channel note

For 8 x ADS1299, use multiple deterministic SPI paths or an FPGA capture front-end if tight synchronization is required. SAMG55 can then handle control, USB and moderate DSP/transport workloads.

See `../DIRECT_USE.md` for the common Microchip flow.