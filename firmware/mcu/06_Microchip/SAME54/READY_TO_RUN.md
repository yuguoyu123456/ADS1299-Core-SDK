# SAME54 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing SAME54 files remain unchanged.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to Harmony 3 SERCOM/SPI, GPIO and EIC resources, then add `../_shared/microchip_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- SERCOM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz during initial validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC input.

## Acquisition pipeline

`DRDY -> short callback -> high-priority acquisition context -> exact 27-byte ADS1299-8 frame -> ring buffer -> Ethernet/USB/storage/DSP/application`

Do not run network, filesystem, logging or signal-processing code in the DRDY callback. Preserve complete frames and sequence counters before downstream processing.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Long capture with transport idle.
7. Repeat under heavy Ethernet/USB/storage load while checking read errors and buffer overflow.
8. Add BIAS, lead-off and faster sample rates only after baseline stability.

## DMAC/cache note

Use DMAC only after the simple SPI path is proven. Keep RX/TX buffers in suitable memory, apply cache maintenance when required, and use explicit ping-pong/ring-buffer ownership.

## 64-channel architecture

For 8 x ADS1299, multiple SPI/DMA lanes or an FPGA capture front-end are preferred for tight synchronization. SAME54 can then handle Ethernet, USB, storage, DSP and packet aggregation without disturbing the sampling path.

See `../DIRECT_USE.md` for the common Microchip integration flow.