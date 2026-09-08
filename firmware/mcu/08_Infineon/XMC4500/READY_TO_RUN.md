# XMC4500 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing XMC4500 files remain unchanged.

## DAVE / XMC Lib integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to USIC SPI, GPIO and ERU/external interrupt resources configured through DAVE or XMC Lib, then add the shared Infineon ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- USIC SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz during first validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge ERU/GPIO interrupt input.

## Acquisition flow

Use the DRDY ISR only to notify a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and places the full frame into a ring buffer.

Keep USB/Ethernet/application work downstream.

## Validation sequence

1. Hardware reset and ID verification.
2. `INFINEON_ADS1299_PROFILE_INTERNAL_TEST`.
3. `INFINEON_ADS1299_PROFILE_INPUT_SHORT`.
4. `INFINEON_ADS1299_PROFILE_EEG_250`.
5. Long continuous capture with frame/header checks.
6. Stress the final transport while monitoring read errors and overflow.
7. Add BIAS, lead-off and higher rates only after stable baseline operation.

## DMA note

After the basic USIC transfer path is proven, DMA can reduce CPU load. Use static ping-pong/ring buffers and explicit ownership between transfer completion and the consumer.

## 64-channel note

For eight ADS1299 devices, multiple SPI/DMA paths or an FPGA capture front-end are preferred when tight inter-device skew matters. XMC4500 can then handle system control and transport.

See `../DIRECT_USE.md` for the common Infineon flow.