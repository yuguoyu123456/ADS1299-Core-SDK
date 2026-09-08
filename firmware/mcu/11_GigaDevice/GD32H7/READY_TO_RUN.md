# GD32H7 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing GD32H7 files remain intact.

## Integration

Reuse `ads1299_port/`, bind it to the selected GD32H7 SPI/GPIO/EXTI resources, and add `../_shared/gd32_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.

## Deterministic capture

The DRDY ISR should only signal the acquisition context. Read exactly one 27-byte ADS1299-8 frame into a DMA-safe ring buffer and keep networking, storage, logging and DSP downstream.

## Validation

1. Hardware reset and device-ID verification.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Sustained capture under final application load.
6. Monitor header alignment, read errors and queue overflow.
7. Add BIAS/lead-off and faster rates only after stability.

## DMA/cache guidance

After a simple transfer path is proven, enable DMA with static ping-pong buffers. Place buffers in DMA-accessible memory and perform any cache clean/invalidate operations required by the selected memory/cache configuration. Keep a non-DMA path available for debugging.

## 64-channel architecture

GD32H7 has strong throughput, but eight tightly synchronized ADS1299 devices still benefit from several independent SPI/DMA lanes or an FPGA capture front-end. The MCU can then handle buffering, DSP and high-speed communication.

See `../DIRECT_USE.md` for the common GD32 integration flow.