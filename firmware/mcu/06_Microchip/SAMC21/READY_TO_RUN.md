# SAMC21 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing SAMC21 files remain intact.

## Harmony 3 integration

Reuse this folder's current `ads1299_port/` and board abstraction. Bind the generic callbacks to Harmony 3 SERCOM/SPI, GPIO and EIC resources generated for the target project, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SERCOM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC input.

## Acquisition flow

The DRDY callback should only signal a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and stores the complete frame in a ring buffer.

Keep CAN, UART, logging and application processing downstream of the sampling path.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture while monitoring frame alignment/read errors/overflow.
7. Add BIAS, lead-off and faster sample rates only after baseline stability.

## DMAC note

Once a simple SERCOM transfer path is proven, DMAC can reduce CPU load. Use fixed ping-pong/ring buffers and explicit ownership between DMA completion and the consumer task.

## 64-channel note

For eight ADS1299 devices, use multiple deterministic SPI/DMA lanes or an FPGA capture front-end when cross-device skew matters. SAMC21 can then handle CAN/industrial transport, control and monitoring.

See `../DIRECT_USE.md` for the common Microchip integration flow.