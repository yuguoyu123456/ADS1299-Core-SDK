# MIMXRT1189 + ADS1299 Ready-to-Run Guide

This is an additive integration guide. Existing files in the MIMXRT1189 directory remain intact.

## Direct integration

1. Keep the existing board and ADS1299 port files in this model directory.
2. Add `../_shared/nxp_ads1299_ready_app.c/.h` plus the common ADS1299 driver/profile sources.
3. Configure the selected LPSPI/SPI controller as master, Mode 1 (CPOL=0, CPHA=1), MSB first.
4. Begin with a 2–4 MHz SPI clock for board bring-up.
5. Configure CS/RESET/PWDN/START outputs and DRDY falling-edge interrupt input.

## Deterministic sampling path

Use DRDY only as the sampling trigger. The interrupt handler should signal a high-priority acquisition task or deferred handler. That context should read exactly 27 bytes for ADS1299-8, update the sequence counter, inspect the status/header and enqueue the complete frame. Networking, IPC, logging and signal processing belong downstream.

For heterogeneous/multicore RT1189 projects, keep the ADS1299 timing-critical path isolated from application/Linux/network workloads as much as the selected system architecture permits.

## First hardware test

- Read and verify ADS1299 ID.
- Run `NXP_ADS1299_PROFILE_INTERNAL_TEST`.
- Run `NXP_ADS1299_PROFILE_INPUT_SHORT`.
- Run `NXP_ADS1299_PROFILE_EEG_250`.
- Confirm continuous frames remain aligned at 27 bytes and status-header decoding remains valid.
- Only then enable BIAS, lead-off and higher data rates.

## DMA and memory coherency

If DMA is enabled, select memory that the DMA master can access and perform the cache maintenance required by the chosen memory/cache configuration. During bring-up, a polling transfer is useful as a reference path before adding DMA.

## 64-channel architecture

For eight ADS1299 devices, prefer FPGA capture or several independent SPI/DMA lanes when channel-to-channel/device-to-device timing must be tightly controlled. MIMXRT1189 can then handle command/control, buffering, timestamping and high-speed uplink.

See `../DIRECT_USE.md` for the reusable NXP integration pattern.