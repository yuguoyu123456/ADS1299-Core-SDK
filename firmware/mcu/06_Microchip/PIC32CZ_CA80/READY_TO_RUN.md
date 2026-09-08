# PIC32CZ CA80 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing PIC32CZ_CA80 files remain unchanged.

## Harmony 3 integration

Reuse this directory's existing `ads1299_port/` and board abstraction. Bind the generic callbacks to Harmony 3 SPI/GPIO/external interrupt resources generated for the target project, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start at approximately 2–4 MHz for initial validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## High-performance acquisition path

Use DRDY only as the real-time trigger. The callback should notify a high-priority acquisition context, which reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and writes the complete frame into a DMA-safe ring buffer.

Keep networking, security, filesystem and DSP workloads downstream.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Sustained capture under representative application load.
7. Monitor header alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher rates only after stable baseline capture.

## DMA/cache note

For DMA-backed SPI, use buffers in DMA-accessible memory and perform any required cache maintenance for the selected memory region. Keep ownership explicit between DMA completion and consumers.

## 64-channel architecture

PIC32CZ CA80 has substantial compute headroom, but for eight ADS1299 devices tightly synchronized capture is still cleaner with multiple SPI/DMA lanes or an FPGA front-end. The MCU can then handle aggregation, DSP, security and high-speed transport.

See `../DIRECT_USE.md` for the common Microchip flow.