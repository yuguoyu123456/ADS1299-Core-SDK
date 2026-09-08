# RP2350 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing RP2350 files remain unchanged.

## Pico SDK integration

Reuse this directory's current `ads1299_port/` abstraction and bind it to Pico SDK SPI, GPIO interrupt and timing functions. Add `../_shared/rp_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Setup

- Hardware SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz during first board validation.
- CS, RESET, PWDN and START outputs.
- DRDY falling-edge GPIO interrupt input.

## Deterministic acquisition

Use DRDY only as a trigger. Keep the GPIO callback minimal and signal a dedicated acquisition context/core. That context reads one exact 27-byte ADS1299-8 frame, updates sequence/error counters and stores the complete frame into a ring buffer.

Use the second core for USB/application/DSP work where practical, but keep ownership of SPI and active DMA buffers explicit.

## Validation

1. Hardware reset ADS1299 and verify ID.
2. `RP_ADS1299_PROFILE_INTERNAL_TEST`.
3. `RP_ADS1299_PROFILE_INPUT_SHORT`.
4. `RP_ADS1299_PROFILE_EEG_250`.
5. Sustained capture with application load minimized.
6. Repeat under representative USB/application/DSP load while checking read errors and queue overflow.
7. Add BIAS/lead-off and faster rates after baseline stability.

## DMA / PIO note

After a standard SPI path is proven, use DMA with fixed ping-pong buffers to reduce CPU occupancy. PIO may help specialized multi-device or deterministic interfaces, but validate it against the standard SPI reference rather than replacing the reference path immediately.

## 64-channel architecture

RP2350 has more headroom than RP2040, but eight tightly synchronized ADS1299 devices still benefit from multiple SPI/DMA paths or an FPGA capture front-end. The MCU can then handle control, buffering, USB and application processing.

See `../DIRECT_USE.md` for the common Raspberry Pi MCU integration flow.