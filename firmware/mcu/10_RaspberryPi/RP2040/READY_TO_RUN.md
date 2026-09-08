# RP2040 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing RP2040 files remain unchanged.

## Pico SDK integration

Reuse the existing `ads1299_port/` abstraction and bind it to Pico SDK SPI, GPIO interrupt and delay functions. Add `../_shared/rp_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Setup

- Hardware SPI master, Mode 1, MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge GPIO interrupt input.

## Acquisition

Keep the GPIO callback short. Signal an acquisition context, read exactly one 27-byte ADS1299-8 frame, update counters and place the full frame into a fixed ring buffer.

A practical dual-core split is: one core owns ADS1299 acquisition, the other handles USB/UART/application work. Do not allow both cores to manipulate the same SPI peripheral or active frame buffer without explicit synchronization.

## Validation

1. Hardware reset and ID verification.
2. `RP_ADS1299_PROFILE_INTERNAL_TEST`.
3. `RP_ADS1299_PROFILE_INPUT_SHORT`.
4. `RP_ADS1299_PROFILE_EEG_250`.
5. Long capture with transport idle, then under representative USB/UART load.
6. Monitor frame alignment, read errors and queue overflow.
7. Add BIAS/lead-off and faster rates after stability.

## DMA / PIO note

First prove the standard hardware SPI path. Then add DMA with static ping-pong buffers if needed. PIO should be treated as an optional optimization for measured timing/parallelism requirements, not a prerequisite for basic ADS1299 capture.

## 64-channel note

For eight ADS1299 devices, FPGA or multiple independent SPI/DMA capture paths are preferred for tight synchronization. RP2040 can then manage control, buffering and USB/host communication.

See `../DIRECT_USE.md` for the common Raspberry Pi MCU flow.