# RA2L1 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing RA2L1 files remain unchanged.

## FSP integration

Reuse the existing `ads1299_port/` and board abstractions in this directory. Bind their generic callbacks to the FSP SPI/GPIO/IRQ instances selected by the RA2L1 project, then add `../_shared/renesas_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, CPOL=0, CPHA=1 (Mode 1), MSB first.
- Start conservatively around 1–2 MHz for low-power bring-up and increase only after reliable capture is proven.
- Configure CS, RESET, PWDN and START as outputs.
- Configure DRDY as a falling-edge external interrupt input.

## Acquisition flow

Use the DRDY callback only to wake or notify a high-priority acquisition context. That context reads one complete 27-byte ADS1299-8 frame, updates counters and queues the frame. Keep logging, communications and signal processing outside the callback.

Recommended flow:

`DRDY -> wake/notify -> 27-byte SPI transfer -> fixed ring buffer -> transport/application -> optional sleep`

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Validate sustained capture under the final low-power policy.
7. Add BIAS, lead-off and faster data rates only after the baseline path is stable.

## Low-power note

Do not enter a power state that disables or delays the DRDY interrupt or the selected SPI instance beyond the sampling requirement. Measure wake-to-transfer latency rather than assuming the sleep policy is safe.

## 64-channel note

RA2L1 is not the preferred sole acquisition engine for 8 x ADS1299 when tight synchronization is required. Use FPGA or a higher-throughput multi-SPI front-end and reserve RA2L1 for control/supervision if appropriate.

See `../DIRECT_USE.md` for the common Renesas flow.