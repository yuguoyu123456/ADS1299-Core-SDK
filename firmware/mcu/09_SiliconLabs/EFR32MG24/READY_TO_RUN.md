# EFR32MG24 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing EFR32MG24 files remain unchanged.

## Gecko SDK integration

Reuse the current `ads1299_port/` abstraction and bind it to Silicon Labs USART/SPIDRV, GPIO interrupt and optional LDMA resources. Add the shared Silicon Labs ready-app and common ADS1299 core/profile sources.

## Setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START outputs.
- DRDY falling-edge GPIO interrupt input.

## Wireless-safe acquisition

The DRDY callback should only signal a high-priority acquisition context. Read one exact 27-byte ADS1299-8 frame, update sequence/error counters and place the frame into a ring buffer before any BLE/Thread/Zigbee/application work.

## Validation

1. Hardware reset and device-ID verification.
2. `SILABS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `SILABS_ADS1299_PROFILE_INPUT_SHORT`.
4. `SILABS_ADS1299_PROFILE_EEG_250`.
5. Long capture with radio idle.
6. Repeat under representative multiprotocol traffic while checking frame alignment, read errors and queue overflow.
7. Add BIAS, lead-off and faster rates only after stable baseline capture.

## LDMA / low-power note

After the synchronous path is known-good, use LDMA with static ping-pong buffers if needed. Verify energy-mode wake latency and keep radio scheduling downstream of acquisition.

## 64-channel architecture

For eight ADS1299 devices, use multiple deterministic SPI/DMA paths or FPGA capture when tight synchronization matters. EFR32MG24 can then handle multiprotocol wireless control and transport.

See `../DIRECT_USE.md` for the common Silicon Labs flow.