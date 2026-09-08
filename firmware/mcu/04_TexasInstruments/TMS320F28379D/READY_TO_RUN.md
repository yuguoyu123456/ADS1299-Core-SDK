# TMS320F28379D + ADS1299 Ready-to-Run Guide

This file only adds practical integration guidance. Existing TMS320F28379D support remains intact.

## C2000 integration

Reuse this directory's current `ads1299_port/` and board abstractions. Bind the generic callbacks to C2000 DriverLib SPI/GPIO resources, then add the shared TI ready-app helper and the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin hardware bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as falling-edge GPIO interrupt.

## Real-time architecture

The F28379D has strong real-time capability, so keep ADS1299 capture deterministic:

`DRDY -> short ISR -> SPI read of one complete frame -> ping-pong/ring buffer -> CPU/CLA processing or transport`

Do not mix printf, host communication or long signal-processing routines into the DRDY ISR. One ADS1299-8 continuous frame is exactly 27 bytes.

## Validation order

1. Hardware reset and device-ID read.
2. `TI_ADS1299_PROFILE_INTERNAL_TEST`.
3. `TI_ADS1299_PROFILE_INPUT_SHORT`.
4. `TI_ADS1299_PROFILE_EEG_250`.
5. Long continuous capture with explicit frame/header checks.
6. Stress the downstream transport or DSP load while checking overflow/read-error counters.
7. Enable BIAS, lead-off and higher rates only after the acquisition path is proven.

## DMA / dual-core note

After a simple SPI path is stable, DMA can be used to reduce CPU load. On dual-core designs, dedicate ownership of the ADS1299 SPI peripheral and capture buffers to one CPU, then pass complete frames to the second CPU through an explicit IPC/buffer protocol.

## 8 x ADS1299 / 64 channels

F28379D is a stronger direct-acquisition candidate than many low-power MCUs, but tight synchronization across eight ADS1299 devices still benefits from multiple independent SPI paths or an FPGA front-end. Use deterministic hardware capture first, then distribute DSP and transport workloads across the C2000 resources.

See `../DIRECT_USE.md` for the shared TI integration flow.