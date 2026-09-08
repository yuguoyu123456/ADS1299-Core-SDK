# SAME70 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing SAME70 files remain unchanged.

## Harmony 3 integration

Reuse this directory's current `ads1299_port/` and board abstraction. Bind the generic callbacks to Harmony 3 SPI/GPIO/external interrupt resources, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for initial hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge interrupt input.

## Deterministic acquisition

Use DRDY only as the real-time trigger. The ISR/callback should notify a high-priority acquisition context, which reads exactly one 27-byte ADS1299-8 frame and stores the complete frame into a DMA-safe ring buffer.

Recommended pipeline:

`DRDY -> short ISR -> 27-byte SPI read -> ring buffer -> Ethernet/USB/storage/DSP/application`

Keep transport and signal processing outside the sampling ISR.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Long continuous capture under representative application load.
7. Monitor frame/header alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher sample rates only after stable baseline capture.

## DMA/cache guidance

After the simple SPI path is proven, use XDMAC/DMA as appropriate with explicit ping-pong/ring-buffer ownership. If cacheable memory is involved, apply the required cache clean/invalidate operations.

## 64-channel architecture

SAME70 has enough performance for serious acquisition work, but for eight ADS1299 devices tight synchronization is still cleaner with multiple independent SPI/DMA lanes or an FPGA front-end. The MCU can then handle aggregation, DSP, Ethernet/USB and storage.

See `../DIRECT_USE.md` for the common Microchip integration flow.