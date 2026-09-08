# RA8M1 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing RA8M1 files remain unchanged.

## FSP integration

Reuse this folder's existing `ads1299_port/` and board abstractions. Bind the generic callbacks to the selected Renesas FSP SPI/GPIO/external IRQ resources, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start board bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition pipeline

`DRDY -> short IRQ callback -> high-priority acquisition context -> exact 27-byte ADS1299-8 read -> ring buffer -> DSP/network/storage/application`

Do not perform network, logging or heavy DSP work in the DRDY callback. Preserve full frames and sequence counters before downstream processing.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Run sustained capture under full application load.
7. Monitor frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher data rates only after stable baseline operation.

## DMA/cache guidance

For DMAC-backed SPI, use DMA-accessible buffers and explicit cache maintenance where required. Keep buffer ownership unambiguous between DMA completion and consumers. A non-DMA baseline should remain available for debugging.

## 8 x ADS1299 / 64 channels

RA8M1 has substantial processing headroom, but tightly synchronized eight-device capture is still cleaner with multiple SPI/DMA lanes or an FPGA front-end. Use the MCU for control, buffering, timestamping, DSP and high-speed transport once deterministic capture is established.

See `../DIRECT_USE.md` for the common Renesas integration flow.