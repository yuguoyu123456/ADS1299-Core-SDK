# GD32VW553 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing GD32VW553 files remain unchanged.

## Integration

Reuse the current `ads1299_port/` abstraction and bind it to the GD32VW553 SPI/GPIO/interrupt resources in the selected SDK. Add `../_shared/gd32_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge interrupt input.

## Acquisition versus wireless work

Keep the DRDY ISR/callback minimal. Signal a high-priority acquisition context, read exactly one 27-byte ADS1299-8 frame and push it into a ring buffer. Wi-Fi/BLE/network packetization and logging must remain downstream.

## Validation

1. Hardware reset and ADS1299 ID verification.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Long capture with wireless idle.
6. Repeat under representative wireless traffic while monitoring frame alignment, read errors and queue overflow.
7. Add BIAS/lead-off and faster rates only after baseline stability.

## DMA note

After a simple synchronous path is proven, use DMA with static ping-pong buffers if needed. Wireless tasks must consume buffered frames without owning the active SPI RX buffer.

## 64-channel architecture

For eight ADS1299 devices, prefer FPGA or multiple deterministic SPI/DMA paths for tight synchronization. GD32VW553 can then focus on wireless control and transport.

See `../DIRECT_USE.md` for the common GD32 flow.