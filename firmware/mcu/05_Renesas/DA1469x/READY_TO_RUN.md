# DA1469x + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing DA1469x files are preserved.

## SmartBond SDK integration

Reuse the current `ads1299_port/` and board abstraction in this directory. Bind the generic SPI/GPIO callbacks to the SmartBond SDK resources already selected by the application, then add `../_shared/renesas_ads1299_ready_app.c/.h` and the common ADS1299 driver/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt.

## Acquisition versus BLE

Use the DRDY callback only to notify a high-priority acquisition task. That task reads exactly one 27-byte ADS1299-8 frame and pushes the complete frame into a ring buffer.

Recommended flow:

`DRDY -> task notification -> 27-byte SPI read -> ring buffer -> BLE/USB/UART/application task`

Do not perform BLE stack operations or packet encoding inside the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture while monitoring read errors and queue overflow.
7. Add BIAS, lead-off and higher data rates only after stable baseline capture.

## Low-power note

If sleep modes are used, ensure the selected mode preserves the interrupt/SPI resources required to service DRDY without violating latency. Acquisition timing must remain independent of BLE connection-event scheduling.

## 64-channel note

For eight ADS1299 devices, use FPGA or multiple deterministic SPI engines when tight inter-device skew matters. DA1469x is better positioned as a low-power control/wireless endpoint than as the sole 64-channel synchronized capture engine.

See `../DIRECT_USE.md` for the common Renesas flow.