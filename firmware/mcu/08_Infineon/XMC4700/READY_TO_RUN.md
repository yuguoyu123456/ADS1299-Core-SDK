# XMC4700 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing XMC4700 files remain unchanged.

## DAVE / XMC Lib integration

Reuse this directory's existing `ads1299_port/` and board support. Bind the generic callbacks to USIC SPI, GPIO and ERU/external interrupt resources configured through DAVE or XMC Lib, then add `../_shared/infineon_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- USIC SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for board bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge ERU/GPIO interrupt input.

## Acquisition pipeline

`DRDY -> short ISR -> high-priority acquisition context -> exact 27-byte ADS1299-8 read -> ring buffer -> Ethernet/USB/CAN/application`

Do not perform logging, networking or signal processing inside the DRDY ISR.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. Run `INFINEON_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `INFINEON_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `INFINEON_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture under representative application/transport load.
7. Monitor frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and faster rates only after stable baseline capture.

## DMA note

After a reliable non-DMA USIC SPI path is established, enable DMA with static ping-pong/ring buffers and explicit ownership between DMA completion and consumers.

## 64-channel note

For 8 x ADS1299, multiple deterministic SPI/DMA lanes or an FPGA front-end remain preferable for tight synchronization. XMC4700 can then handle aggregation, industrial transport and control.

See `../DIRECT_USE.md` for the common Infineon integration flow.