# nRF52840 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing nRF52840 files remain unchanged.

## SDK integration

Reuse this folder's current `ads1299_port/` and board abstraction. Bind the generic callbacks to nRF Connect SDK/Zephyr SPI/GPIO APIs or the legacy nRF5 SDK SPIM/GPIOTE implementation already used by the project, then add `../_shared/nordic_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPIM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO/GPIOTE input.

## Acquisition pipeline

`DRDY -> short callback -> high-priority acquisition context -> exact 27-byte ADS1299-8 SPIM read -> ring buffer -> BLE/USB/UART/application`

Do not perform BLE notifications, USB writes, logging or signal processing inside the DRDY callback. Preserve a complete frame and sequence counter before downstream work begins.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Long capture with BLE/USB idle.
7. Repeat under representative BLE/USB traffic while monitoring frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher sample rates only after stable baseline capture.

## EasyDMA / PPI guidance

Once the simple SPIM path is proven, EasyDMA can reduce CPU load. Static ping-pong buffers are preferred. GPIOTE/PPI may be used to reduce trigger latency/jitter, but keep a software-triggered reference implementation for validation and debugging.

## 64-channel note

For eight ADS1299 devices, multiple SPIM/DMA paths or an FPGA capture stage are preferred for tight synchronization. nRF52840 is then well suited to BLE/USB control and transport.

See `../DIRECT_USE.md` for the common Nordic integration flow.