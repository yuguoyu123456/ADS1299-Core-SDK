# MSPM0G3507 + ADS1299 Ready-to-Run Guide

This guide only adds integration material; all existing MSPM0G3507 files remain intact.

## SDK integration

Reuse the existing `ads1299_port/` and board files. Bind the generic callbacks to the MSPM0 DriverLib / SysConfig SPI and GPIO resources already used by the target project, then add the shared TI ready-app and the common ADS1299 driver/profile sources.

## Peripheral setup

- SPI master, CPOL=0, CPHA=1 (SPI Mode 1), MSB first.
- Start at roughly 2–4 MHz during board bring-up.
- CS/RESET/PWDN/START as outputs.
- DRDY as falling-edge interrupt input.

## Recommended runtime path

`DRDY -> short ISR -> acquisition task/deferred handler -> exact 27-byte ADS1299-8 frame -> ring buffer -> UART/USB/CAN/application`

The ISR should not perform logging, packet formatting or signal processing. Keep complete ADS1299 frames intact until they are safely queued.

## Validation order

1. Hardware reset.
2. Verify ADS1299 ID.
3. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `TI_ADS1299_PROFILE_EEG_250`.
6. Run a sustained capture and monitor read-error/queue-overflow counters.
7. Add BIAS, lead-off and higher sample rates only after baseline stability.

## DMA note

MSPM0G3507 can use DMA to reduce SPI CPU overhead. Establish a known-good polling/interrupt baseline first, then enable DMA with explicit ping-pong/ring-buffer ownership so a consumer can never overwrite an active RX buffer.

## 8 x ADS1299 / 64 channels

For 64 channels, use multiple SPI/DMA engines or an FPGA capture front-end if device-to-device skew must remain tightly controlled. MSPM0G3507 can then handle configuration, monitoring and transport.

See `../DIRECT_USE.md` for the shared TI sequence.