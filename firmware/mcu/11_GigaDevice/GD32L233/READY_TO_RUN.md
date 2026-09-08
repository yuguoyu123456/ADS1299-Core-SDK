# GD32L233 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing GD32L233 files remain unchanged.

## Integration

Reuse the existing `ads1299_port/` abstraction and bind it to GD32L23x SPI/GPIO/EXTI resources. Add the shared GD32 ready-app and common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1, MSB first.
- Start around 1–2 MHz for low-power bring-up; increase after stable validation.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.

## Low-power acquisition

Use DRDY to wake/notify the acquisition context. Read exactly one 27-byte ADS1299-8 frame into a static ring buffer before optional return to sleep. Do not use a power state that delays DRDY/SPI service beyond the sample-period budget.

## Validation

1. Hardware reset and ID verification.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Sustained capture under the final sleep/wake policy.
6. Monitor read errors and queue overflow.
7. Add BIAS/lead-off and faster rates after stability.

## 64-channel note

For eight ADS1299 devices, GD32L233 is better used as a low-power control/supervisory processor. Use FPGA or a higher-throughput multi-SPI front-end for synchronized 64-channel capture.

See `../DIRECT_USE.md` for the common GD32 flow.