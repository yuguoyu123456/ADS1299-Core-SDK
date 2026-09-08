# EFM32GG11 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing EFM32GG11 files remain intact.

## Integration

Reuse the current `ads1299_port/` abstraction and bind it to Gecko SDK/emlib USART synchronous-master, GPIO interrupt and optional LDMA resources. Add the shared Silicon Labs ready-app and common ADS1299 core/profile sources.

## Setup

- USART/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for hardware bring-up.
- CS, RESET, PWDN and START outputs.
- DRDY falling-edge GPIO interrupt input.

## Acquisition path

The DRDY ISR should only notify a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and places the full frame in a ring buffer.

Keep USB/Ethernet/logging and DSP downstream.

## Validation

1. Hardware reset and ADS1299 ID verification.
2. `SILABS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `SILABS_ADS1299_PROFILE_INPUT_SHORT`.
4. `SILABS_ADS1299_PROFILE_EEG_250`.
5. Long continuous capture while monitoring frame alignment, read errors and queue overflow.
6. Stress final transport.
7. Add BIAS/lead-off and higher rates after stability.

## LDMA note

After a simple USART/SPI path is proven, LDMA can reduce CPU load. Use static ping-pong/ring buffers with explicit ownership between DMA completion and consumers.

## 64-channel note

For eight ADS1299 devices, multiple deterministic SPI/DMA paths or FPGA capture are preferred for tight synchronization. EFM32GG11 can then handle control and buffered transport.

See `../DIRECT_USE.md` for the common Silicon Labs flow.