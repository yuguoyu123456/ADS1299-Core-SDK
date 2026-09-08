# RX72N + ADS1299 Ready-to-Run Guide

This file is additive only. Existing RX72N files remain unchanged.

## RX integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to RSPI/GPIO/external interrupt resources configured through FIT/Smart Configurator or the project’s low-level driver, then add the shared Renesas ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- RSPI/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin hardware bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition pipeline

`DRDY -> short ISR -> high-priority acquisition context -> exact 27-byte ADS1299-8 frame -> ring buffer -> Ethernet/USB/storage/DSP/application`

Do not let Ethernet, filesystem, logging or DSP work execute in the DRDY ISR. Preserve complete frames and sequence counters before downstream processing.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Long continuous capture with transport idle.
7. Repeat under heavy Ethernet/USB/storage load while checking read errors and queue overflow.
8. Add BIAS, lead-off and higher data rates only after stable baseline capture.

## DMA and transport note

RX72N has enough performance for buffered high-speed transport. After a stable polling/interrupt baseline, use DMA/DTC where appropriate and explicit ping-pong/ring-buffer ownership to isolate acquisition from network/filesystem latency.

## 64-channel architecture

For eight ADS1299 devices, multiple SPI/DMA paths or an FPGA capture stage remain preferable for tight synchronization. RX72N can then handle packetization, networking, storage and signal processing without disturbing the real-time sampling path.

See `../DIRECT_USE.md` for the common Renesas flow.