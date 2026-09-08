# PIC32MZ EF + ADS1299 Ready-to-Run Guide

This file is additive only. Existing PIC32MZ_EF files remain unchanged.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to the Harmony 3 SPI/GPIO/external interrupt implementation used by the project, then add `../_shared/microchip_ads1299_ready_app.c/.h` plus the common ADS1299 driver/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz during first hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition architecture

Recommended pipeline:

`DRDY -> short ISR -> high-priority acquisition context -> exact 27-byte ADS1299-8 read -> DMA-safe ring buffer -> Ethernet/USB/storage/DSP/application`

Do not let network stacks, filesystems or DSP routines execute in the DRDY ISR.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Perform long continuous capture with transport idle.
7. Repeat under heavy transport/application load while monitoring read errors and queue overflow.
8. Add BIAS, lead-off and higher rates only after stable baseline operation.

## DMA/cache guidance

PIC32MZ EF commonly uses cached memory and DMA. Put DMA buffers in an appropriate memory region and use the required cache clean/invalidate operations. Keep a simple non-DMA transfer path available to isolate cache/DMA issues from ADS1299 SPI timing issues.

## 64-channel architecture

For 8 x ADS1299, multiple independent SPI/DMA lanes or an FPGA front-end are preferred when tight device-to-device skew matters. PIC32MZ EF can then handle packet aggregation, DSP, Ethernet/USB and storage without disturbing sampling.

See `../DIRECT_USE.md` for the common Microchip flow.