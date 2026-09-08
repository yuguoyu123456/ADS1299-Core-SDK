# RA4E2 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing RA4E2 files remain unchanged.

## FSP integration

Reuse this directory's existing `ads1299_port/` and board abstraction. Bind the generic callbacks to the Renesas FSP SPI/GPIO/external IRQ instances generated for the project, then add the shared Renesas ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start board bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition architecture

Keep the DRDY callback short: acknowledge the IRQ and notify a high-priority acquisition context. That context performs one complete 27-byte ADS1299-8 frame transfer, validates/stores the frame and pushes it into a ring buffer.

UART/USB/CAN/application processing should consume the ring buffer downstream and must not block sample servicing.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Long continuous capture with read-error/overflow counters monitored.
7. Add BIAS, lead-off and faster sample rates only after baseline stability.

## DMA/DTC note

First prove a simple SPI path. Then use DTC/DMAC if needed, with fixed buffer ownership and explicit completion signaling so the consumer never races the active transfer buffer.

## 64-channel note

For 8 x ADS1299, prefer multiple deterministic SPI/DMA paths or an FPGA front-end when device-to-device skew matters. RA4E2 can then manage configuration, buffering and transport.

See `../DIRECT_USE.md` for the common Renesas integration flow.