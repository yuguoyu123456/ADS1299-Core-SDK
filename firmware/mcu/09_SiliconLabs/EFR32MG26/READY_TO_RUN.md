# EFR32MG26 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing EFR32MG26 files remain unchanged.

## Gecko SDK integration

Reuse this directory's `ads1299_port/` abstraction and bind it to Silicon Labs USART/SPIDRV, GPIO interrupt and optional LDMA resources. Add `../_shared/silabs_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for initial validation.
- CS/RESET/PWDN/START outputs; DRDY falling-edge GPIO interrupt input.

## Acquisition architecture

Use DRDY only as the sampling trigger. The callback should notify a high-priority acquisition context, which reads exactly one 27-byte ADS1299-8 frame, updates counters and queues the complete frame.

Keep Bluetooth/Thread/Zigbee/application processing downstream.

## Validation

1. Hardware reset ADS1299 and verify ID.
2. `SILABS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `SILABS_ADS1299_PROFILE_INPUT_SHORT`.
4. `SILABS_ADS1299_PROFILE_EEG_250`.
5. Long capture with radio idle and again under representative wireless load.
6. Monitor frame/header alignment, read errors and queue overflow.
7. Add BIAS/lead-off and higher rates only after baseline stability.

## LDMA / low-power note

Use LDMA only after a simple synchronous reference path is proven. Maintain explicit ping-pong buffer ownership and verify any selected energy mode preserves the required DRDY-to-SPI response time.

## 64-channel note

For 8 x ADS1299, multiple deterministic SPI/DMA lanes or FPGA capture remain preferred for tight cross-device synchronization. EFR32MG26 can then provide multiprotocol wireless control and uplink.

See `../DIRECT_USE.md` for the common Silicon Labs integration flow.