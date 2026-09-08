# EFM32 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing EFM32 files remain unchanged.

## Integration

Reuse the existing `ads1299_port/` layer and bind it to Gecko SDK/emlib USART synchronous-master and GPIO interrupt resources. Add `../_shared/silabs_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Setup

- SPI/USART synchronous master, Mode 1, MSB first.
- Start around 1–2 MHz for low-power parts, then increase after validation.
- CS/RESET/PWDN/START outputs; DRDY falling-edge interrupt input.

## Acquisition

`DRDY -> short GPIO ISR -> high-priority acquisition context -> one exact 27-byte frame -> static ring buffer -> UART/USB/application`

Avoid logging, floating-point conversion and long transport work in the DRDY ISR.

## Validation

1. Reset ADS1299 and verify ID.
2. `SILABS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `SILABS_ADS1299_PROFILE_INPUT_SHORT`.
4. `SILABS_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with frame/header and overflow monitoring.
6. Add BIAS/lead-off and faster rates only after stability.

## Low-power / DMA note

If sleep modes are used, verify wake-to-SPI latency. Where LDMA is available, introduce it only after the basic synchronous path is known-good, with explicit buffer ownership.

## 64-channel note

For eight ADS1299 devices, use FPGA or multiple deterministic SPI/DMA paths when tight synchronization matters. EFM32 can serve control and low-power supervisory roles.

See `../DIRECT_USE.md` for the common Silicon Labs flow.