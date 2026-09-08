# GD32E230 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing GD32E230 files remain unchanged.

## Integration

Reuse `ads1299_port/`, bind it to the GD32E23x SPI/GPIO/EXTI implementation, and add `../_shared/gd32_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1, MSB first.
- Start around 1–2 MHz and increase only after stable validation.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.

## Acquisition

Keep the DRDY ISR minimal. Signal the foreground/high-priority acquisition context, read exactly one 27-byte ADS1299-8 frame and store it in a fixed-size ring buffer. Avoid dynamic allocation, floating-point conversion and printf in the sampling path.

## Validation

1. Hardware reset and ID verification.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with frame alignment/read-error/overflow monitoring.
6. Add BIAS/lead-off and faster rates only after stability.

## 64-channel note

GD32E230 is not the preferred sole capture engine for 8 x ADS1299. Use FPGA or a higher-throughput multi-SPI front-end for synchronized 64-channel acquisition.

See `../DIRECT_USE.md` for the common GD32 flow.