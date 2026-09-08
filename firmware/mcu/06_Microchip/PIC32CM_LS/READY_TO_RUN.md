# PIC32CM LS + ADS1299 Ready-to-Run Guide

This file only adds practical integration guidance. Existing PIC32CM_LS files remain intact.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to Harmony 3 SERCOM/SPI, GPIO and EIC resources generated for the target project, then add the shared Microchip ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC/external interrupt input.

## Acquisition path

Keep the DRDY callback minimal and use it only to notify a high-priority acquisition context. That context reads one complete 27-byte ADS1299-8 frame, updates counters and pushes the frame into a static ring buffer.

Security/application/transport work should remain downstream so it cannot block sampling.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Confirm sustained frame alignment and no silent queue overflow.
7. Add BIAS, lead-off and faster rates only after baseline stability.

## DMA note

After the simple path is stable, use DMAC with explicit buffer ownership. Keep a non-DMA reference path available for debugging SPI/frame alignment issues.

## 64-channel note

For 8 x ADS1299, use an FPGA or several deterministic SPI/DMA lanes if cross-device skew is important. PIC32CM LS can then handle secure control and moderate-rate transport.

See `../DIRECT_USE.md` for the common Microchip flow.