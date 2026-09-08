# CC2340R5 + ADS1299 Ready-to-Run Guide

This file only adds direct-use guidance. Existing CC2340R5 support remains intact.

## SDK setup

Reuse the current `ads1299_port/` and board files in this directory. Bind the generic ADS1299 port callbacks to the SimpleLink / TI Drivers SPI and GPIO functions used by the target project.

Add `../_shared/ti_ads1299_ready_app.c/.h` and the ADS1299 core/profile sources.

## Peripheral requirements

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Conservative 2–4 MHz clock for initial validation.
- CS/RESET/PWDN/START outputs.
- DRDY falling-edge interrupt.

## Acquisition task

Use the DRDY callback only as a notification source. A high-priority task should perform the exact 27-byte ADS1299-8 transfer and immediately place the complete frame into a fixed-size queue or ring buffer.

BLE packet construction, GATT notifications, logging and signal processing should run in separate lower-priority contexts.

## Validation order

1. Hardware reset and ID verification.
2. `TI_ADS1299_PROFILE_INTERNAL_TEST`.
3. `TI_ADS1299_PROFILE_INPUT_SHORT`.
4. `TI_ADS1299_PROFILE_EEG_250`.
5. Confirm sustained frame alignment and zero or visible queue-overflow events.
6. Add BIAS, lead-off and higher data rates after the basic stream is stable.

## BLE-oriented note

Because CC2340R5 is commonly used in BLE-oriented systems, treat BLE throughput as a downstream transport constraint rather than allowing it to block sampling. Buffer samples, aggregate packets where appropriate, and keep acquisition timing independent of connection-event timing.

## 64-channel note

For eight ADS1299 devices, prefer FPGA or multiple SPI engines for deterministic capture. CC2340R5 is more suitable as a BLE/control endpoint than as the sole tight-skew 64-channel acquisition engine.

See `../DIRECT_USE.md` for the common TI flow.