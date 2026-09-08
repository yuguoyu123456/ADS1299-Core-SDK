# SAMD51 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing SAMD51 files remain unchanged.

## Harmony 3 integration

Reuse this directory's current `ads1299_port/` and board abstraction. Bind the generic callbacks to Harmony 3 SERCOM/SPI, GPIO and EIC resources, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SERCOM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin at approximately 2–4 MHz during board bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC input.

## Acquisition architecture

Use the DRDY callback only to notify a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and places the complete frame into a ring buffer.

Keep USB, storage, DSP and application work downstream.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Perform long continuous capture under representative transport/DSP load.
7. Monitor header alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher sample rates only after stable baseline capture.

## DMAC/cache note

After a simple transfer path is proven, use DMAC with explicit ping-pong/ring-buffer ownership. If the selected memory/cache configuration affects DMA coherency, apply the required maintenance and keep a non-DMA baseline for debugging.

## 64-channel note

For eight ADS1299 devices, multiple independent SPI/DMA lanes or an FPGA front-end are preferred when cross-device timing matters. SAMD51 can then handle DSP, USB, storage and control.

See `../DIRECT_USE.md` for the common Microchip flow.