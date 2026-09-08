# XMC7200 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing XMC7200 files remain intact.

## ModusToolbox / PDL integration

Reuse the existing `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to SCB SPI, GPIO and interrupt/DMA resources configured through ModusToolbox/PDL, then add the shared Infineon ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SCB SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz during hardware bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt input.

## Deterministic acquisition

Use the DRDY callback only to signal a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates sequence/error counters and stores the frame in a DMA-safe ring buffer.

Keep CAN/Ethernet/application/control workloads downstream of sampling.

## Validation sequence

1. Hardware reset and device-ID verification.
2. `INFINEON_ADS1299_PROFILE_INTERNAL_TEST`.
3. `INFINEON_ADS1299_PROFILE_INPUT_SHORT`.
4. `INFINEON_ADS1299_PROFILE_EEG_250`.
5. Sustained capture under representative application load.
6. Monitor frame/header alignment, read errors and queue overflow.
7. Add BIAS, lead-off and higher rates only after stable baseline capture.

## DMA/cache note

After proving a simple SCB SPI path, use DMA where useful with explicit ping-pong/ring-buffer ownership. If cached memory is involved, ensure DMA coherency is handled correctly and retain a non-DMA baseline for debugging.

## 64-channel architecture

XMC7200 has strong real-time capability, but for eight ADS1299 devices tightly synchronized acquisition is still cleaner with multiple independent SPI/DMA paths or an FPGA capture front-end. Use the MCU for control, buffering, industrial communication and processing after deterministic capture is established.

See `../DIRECT_USE.md` for the common Infineon integration flow.