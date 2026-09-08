# SAMV71 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing SAMV71 files remain unchanged.

## Harmony 3 integration

Reuse this directory's current `ads1299_port/` and board abstraction. Bind the generic callbacks to Harmony 3 SPI/GPIO/external interrupt resources, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for board bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge interrupt input.

## Deterministic acquisition

Recommended pipeline:

`DRDY -> short ISR -> high-priority acquisition context -> exact 27-byte ADS1299-8 read -> DMA-safe ring buffer -> Ethernet/CAN/USB/storage/DSP/application`

Keep network, CAN, storage and DSP workloads outside the DRDY ISR. Store complete frames and sequence counters before downstream processing.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Long continuous capture under representative Ethernet/CAN/application load.
7. Monitor header alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher rates only after stable baseline capture.

## DMA/cache guidance

After a simple SPI path is proven, use XDMAC/DMA with explicit ping-pong/ring-buffer ownership. If cacheable memory is used, apply the required cache clean/invalidate operations and keep a non-DMA reference path for debugging.

## 64-channel architecture

SAMV71 has strong throughput for acquisition and transport, but tightly synchronized eight-device capture is still cleaner with multiple independent SPI/DMA paths or an FPGA front-end. The MCU can then handle aggregation, CAN/Ethernet, DSP, storage and system control.

See `../DIRECT_USE.md` for the common Microchip integration flow.