# MSPM0L1306 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing MSPM0L1306 support files remain unchanged.

## Integration

Reuse this directory's current `ads1299_port/` and board abstractions. Bind the generic callbacks to MSPM0 DriverLib / SysConfig-generated SPI and GPIO resources, then add the shared TI ready-app and common ADS1299 driver/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin at a conservative 1–2 MHz if the selected low-power clock configuration is modest; increase after stable validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge interrupt input.

## Low-power acquisition pattern

Keep the DRDY ISR short. It should wake or notify the acquisition context, which then reads one complete 27-byte ADS1299-8 frame and queues it. Avoid entering a low-power mode that prevents DRDY or SPI servicing at the required latency.

Recommended flow:

`DRDY -> wake/notify -> 27-byte read -> fixed ring buffer -> transport task -> optional sleep`

## Bring-up sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `TI_ADS1299_PROFILE_EEG_250`.
6. Validate sustained capture with the final low-power policy enabled.
7. Add BIAS, lead-off and faster rates after the basic path is stable.

## 64-channel note

For 8 x ADS1299, do not rely on a low-power MSPM0L device as the sole tight-skew capture engine. Prefer FPGA or multiple dedicated SPI acquisition paths and use MSPM0L1306 for control/supervisory functions when appropriate.

See `../DIRECT_USE.md` for the common TI flow.