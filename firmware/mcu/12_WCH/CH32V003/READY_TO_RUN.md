# CH32V003 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing CH32V003 files remain unchanged.

## Integration

Reuse `ads1299_port/`, bind it to the CH32V003 SPI/GPIO/EXTI implementation, and add `../_shared/wch_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Resource-conscious setup

- SPI master, Mode 1, MSB first.
- Start conservatively around 0.5–1 MHz and increase only after stable validation.
- CS/RESET/PWDN/START outputs; DRDY falling-edge interrupt input.

Keep the DRDY ISR minimal. Read exactly one 27-byte ADS1299-8 frame in the foreground/high-priority acquisition path and use small static buffers. Avoid floating-point conversion, printf and dynamic allocation during sampling.

## Validation

1. Hardware reset and ID verification.
2. `WCH_ADS1299_PROFILE_INTERNAL_TEST`.
3. `WCH_ADS1299_PROFILE_INPUT_SHORT`.
4. `WCH_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with frame alignment and overflow monitoring.
6. Add optional BIAS/lead-off only after stability.

## 64-channel note

CH32V003 is not intended as the primary synchronized acquisition engine for eight ADS1299 devices. Use FPGA or a higher-throughput MCU/front-end for 64 channels.

See `../DIRECT_USE.md` for the common WCH flow.