# PSoC 6 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing PSoC6 files remain unchanged.

## ModusToolbox integration

Reuse this directory's existing `ads1299_port/` and board abstraction. Bind the generic callbacks to ModusToolbox HAL/PDL SPI, GPIO and interrupt resources, then add `../_shared/infineon_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START as outputs.
- DRDY as a falling-edge GPIO interrupt.

## Acquisition architecture

Keep the DRDY callback short and signal a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame and pushes the complete frame into a ring buffer.

Recommended flow:

`DRDY -> event/semaphore -> 27-byte SPI read -> ring buffer -> BLE/USB/application`

Do not perform BLE, logging or DSP work in the DRDY callback.

## Validation order

1. Hardware reset and ADS1299 ID read.
2. `INFINEON_ADS1299_PROFILE_INTERNAL_TEST`.
3. `INFINEON_ADS1299_PROFILE_INPUT_SHORT`.
4. `INFINEON_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with communications idle, then under representative transport load.
6. Monitor frame alignment, read errors and queue overflow.
7. Add BIAS, lead-off and higher rates after baseline stability.

## DMA / dual-core note

After a simple SPI path is proven, use DMA if needed with explicit buffer ownership. If the application uses both PSoC 6 cores, keep deterministic acquisition ownership clear and pass complete frames between contexts instead of sharing an active SPI buffer.

## 64-channel note

For 8 x ADS1299, use multiple deterministic SPI/DMA lanes or FPGA capture if tight synchronization is required. PSoC 6 can then handle control, BLE, USB and application processing.

See `../DIRECT_USE.md` for the common Infineon flow.