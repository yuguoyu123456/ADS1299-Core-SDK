# GD32F350 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing GD32F350 files remain unchanged.

## Integration

Reuse `ads1299_port/`, bind it to the GD32F3x0 SPI/GPIO/EXTI implementation, and add `../_shared/gd32_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1, MSB first.
- Begin around 2–4 MHz for hardware bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.

## Acquisition

Keep the DRDY ISR short and signal a high-priority acquisition context. Read exactly one 27-byte ADS1299-8 frame, update sequence/error counters and queue the complete frame before UART/USB/application processing.

## Validation

1. Hardware reset and ADS1299 ID read.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with frame alignment/read-error/overflow monitoring.
6. Add BIAS/lead-off and higher rates only after stable baseline capture.

## DMA note

Introduce DMA only after the simple SPI path is proven. Use static ping-pong/ring buffers with explicit ownership.

## 64-channel note

For eight ADS1299 devices, prefer FPGA or multiple deterministic SPI/DMA lanes when cross-device skew matters.

See `../DIRECT_USE.md` for the common GD32 flow.