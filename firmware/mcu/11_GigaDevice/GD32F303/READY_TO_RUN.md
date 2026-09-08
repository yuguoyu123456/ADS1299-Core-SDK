# GD32F303 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing GD32F303 files remain intact.

## Integration

Reuse the existing `ads1299_port/` abstraction and bind it to GD32F30x SPI, GPIO and EXTI resources. Add the shared GD32 ready-app and common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for first validation.
- CS, RESET, PWDN and START outputs; DRDY falling-edge EXTI input.

## Acquisition path

`DRDY -> short EXTI ISR -> high-priority acquisition context -> exact 27-byte ADS1299-8 read -> ring buffer -> UART/USB/CAN/application`

Do not perform logging or signal processing in the DRDY ISR.

## Validation

1. Hardware reset and ID verification.
2. `GD32_ADS1299_PROFILE_INTERNAL_TEST`.
3. `GD32_ADS1299_PROFILE_INPUT_SHORT`.
4. `GD32_ADS1299_PROFILE_EEG_250`.
5. Long continuous capture with frame/header and overflow monitoring.
6. Add BIAS/lead-off and higher rates after baseline stability.

## DMA note

After a reliable synchronous SPI path is established, use DMA with static ping-pong buffers and explicit ownership between DMA completion and consumers.

## 64-channel note

For 8 x ADS1299, multiple independent SPI/DMA lanes or FPGA capture are preferred when tight synchronization matters.

See `../DIRECT_USE.md` for the common GD32 integration flow.