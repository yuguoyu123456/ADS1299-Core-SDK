# EFR32BG22 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing EFR32BG22 files remain unchanged.

## Gecko SDK integration

Reuse the existing `ads1299_port/` abstraction and bind it to USART/SPIDRV, GPIO interrupt and optional LDMA resources in the Silicon Labs SDK. Add `../_shared/silabs_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge GPIO interrupt input.

## Acquisition versus BLE

Use the DRDY callback only to notify a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame and enqueues it.

`DRDY -> acquisition event -> 27-byte transfer -> ring buffer -> Bluetooth/application task`

Do not perform Bluetooth notifications or logging in the DRDY callback.

## Validation

1. Hardware reset and ID verification.
2. `SILABS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `SILABS_ADS1299_PROFILE_INPUT_SHORT`.
4. `SILABS_ADS1299_PROFILE_EEG_250`.
5. Long capture with radio idle, then under representative BLE traffic.
6. Monitor read errors and queue overflow.
7. Add BIAS/lead-off and faster rates after baseline stability.

## LDMA / low-power note

Introduce LDMA only after a synchronous reference path is stable. If energy modes are used, verify wake latency and ensure radio scheduling cannot delay ADS1299 service beyond the required budget.

## 64-channel note

For 8 x ADS1299, prefer FPGA or multiple deterministic SPI/DMA lanes when tight synchronization matters. EFR32BG22 can then focus on BLE control and transport.

See `../DIRECT_USE.md` for the common Silicon Labs flow.