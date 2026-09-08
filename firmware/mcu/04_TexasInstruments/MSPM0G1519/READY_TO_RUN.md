# MSPM0G1519 + ADS1299 Ready-to-Run Guide

This file is additive only and leaves all existing MSPM0G1519 files unchanged.

## SDK integration

Reuse the current `ads1299_port/` and board abstractions. Bind the generic callbacks to MSPM0 DriverLib / SysConfig-generated SPI and GPIO resources, then add `../_shared/ti_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## SPI / GPIO setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin at approximately 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge interrupt input.

## Acquisition architecture

The DRDY ISR should only clear the interrupt and signal a high-priority acquisition context. That context performs one complete 27-byte ADS1299-8 transfer, updates the sample sequence/error counters and writes the frame into a fixed-size ring buffer.

UART/USB/CAN/application work should consume the ring buffer asynchronously and must not block the DRDY service path.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. `TI_ADS1299_PROFILE_EEG_250`.
6. Run a sustained capture and confirm no silent drops.
7. Enable BIAS, lead-off and higher rates after baseline validation.

## DMA note

If DMA is used for SPI, validate the simpler interrupt/polling path first. Then introduce DMA with explicit buffer ownership and queue-overflow accounting.

## 64-channel note

For eight ADS1299 devices, FPGA or several independent SPI/DMA paths are preferred when synchronization matters. MSPM0G1519 can still provide control, health monitoring and moderate-rate transport.

See `../DIRECT_USE.md` for the common TI flow.