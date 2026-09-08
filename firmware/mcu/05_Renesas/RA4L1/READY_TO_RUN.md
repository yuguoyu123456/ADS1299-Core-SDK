# RA4L1 + ADS1299 Ready-to-Run Guide

This guide only adds integration material. Existing RA4L1 files remain intact.

## FSP integration

Reuse the existing `ads1299_port/` and board support in this directory. Bind the generic ADS1299 callbacks to the selected Renesas FSP SPI/GPIO/external IRQ resources, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, CPOL=0, CPHA=1 (Mode 1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START as outputs.
- DRDY as a falling-edge external interrupt input.

## Low-power acquisition pattern

The DRDY callback should only wake or notify the acquisition context. That context reads one complete 27-byte ADS1299-8 frame, updates counters and places the complete frame into a ring buffer.

Recommended flow:

`DRDY -> wake/notify -> 27-byte SPI read -> ring buffer -> transport/application -> optional low-power entry`

Avoid logging, signal processing and long transport work inside the DRDY callback.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Test sustained capture with the intended low-power policy active.
7. Add BIAS, lead-off and higher rates only after stable baseline capture.

## Low-power timing note

Measure wake latency and confirm the SPI/IRQ resources are available early enough to service every DRDY event. Do not assume a low-power mode is compatible until the complete stream is stress-tested.

## 64-channel note

For eight ADS1299 devices, use FPGA or a higher-throughput multi-SPI architecture if tight synchronization matters. RA4L1 can remain a low-power control and monitoring processor.

See `../DIRECT_USE.md` for the common Renesas flow.