# GD32F450 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing GD32F450 files remain unchanged.

## Integration

Reuse the current `ads1299_port/` abstraction and bind it to GD32F4xx SPI/GPIO/EXTI resources. Add the shared GD32 ready-app and common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1, MSB first.
- Start around 2–4 MHz.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.

## Acquisition

Use DRDY only to notify a high-priority acquisition context. Read exactly one 27-byte ADS1299-8 frame, update sequence/error counters and place the full frame into a ring buffer before transport or DSP work.

## Validation

1. Reset ADS1299 and verify ID.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Long capture under representative USB/Ethernet/application load.
6. Monitor header alignment, read errors and queue overflow.
7. Add BIAS/lead-off and higher rates after stability.

## DMA note

After a synchronous reference path is proven, use DMA with explicit ping-pong/ring-buffer ownership.

## 64-channel architecture

For 8 x ADS1299, multiple SPI/DMA lanes or an FPGA front-end are preferred when tight synchronization matters. GD32F450 can then handle aggregation and transport.

See `../DIRECT_USE.md` for the common GD32 flow.