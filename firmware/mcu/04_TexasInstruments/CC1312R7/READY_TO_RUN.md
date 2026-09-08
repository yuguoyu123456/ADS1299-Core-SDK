# CC1312R7 + ADS1299 Ready-to-Run Guide

This file extends the existing CC1312R7 support without deleting or replacing any current files.

## SDK integration

Use the existing `ads1299_port/` and `board/` layers in this directory. Bind the generic SPI/GPIO callbacks to SimpleLink / TI Drivers or to the project’s existing low-level driver implementation.

Add the common helper sources from `../_shared/` plus the ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start bring-up around 2–4 MHz.
- CS, RESET, PWDN and START as outputs.
- DRDY as falling-edge GPIO interrupt.
- Keep the radio stack and ADS1299 acquisition path decoupled.

## Runtime architecture

Recommended flow:

`DRDY callback -> task/event notification -> exact 27-byte ADS1299-8 transfer -> ring buffer -> Sub-1 GHz/BLE/UART transport task`

The GPIO callback should only acknowledge the interrupt and notify the acquisition task. Do not call radio APIs, printf or packet encoding inside the DRDY callback.

## Bring-up sequence

1. Hardware-reset ADS1299.
2. Read and verify the ID register.
3. Apply `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Apply `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Apply `TI_ADS1299_PROFILE_EEG_250`.
6. Verify long continuous frame alignment and queue-overflow counters.
7. Only then enable BIAS/lead-off and higher rates.

## Low-power/radio note

CC1312R7 is attractive for wireless links, but RF scheduling and low-power state transitions must not delay the DRDY servicing path. Keep acquisition in a high-priority task and let a separate communication task consume buffered frames.

## 64-channel note

For 8 x ADS1299, prefer a deterministic FPGA or multi-SPI capture front-end and use CC1312R7 for control/telemetry rather than assuming one radio-oriented MCU should directly service all 64 channels with tight skew.

See `../DIRECT_USE.md` for the common TI flow.