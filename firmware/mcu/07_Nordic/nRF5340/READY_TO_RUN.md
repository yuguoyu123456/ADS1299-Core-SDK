# nRF5340 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing nRF5340 files remain intact.

## nRF Connect SDK integration

Reuse the existing `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to Zephyr/nRF Connect SDK SPIM and GPIO resources selected through devicetree, then add `../_shared/nordic_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- SPIM master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for first hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt input.

## Dual-core architecture

Keep deterministic ADS1299 acquisition on the application-core side and keep network-core/radio work decoupled through queues or the SDK's established IPC mechanisms. The DRDY callback should only notify a high-priority acquisition thread/work item.

Recommended flow:

`DRDY -> acquisition notification -> exact 27-byte SPIM frame -> ring buffer -> BLE/IPC/USB/application consumer`

Do not perform radio protocol work in the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Run long capture with radio idle.
7. Repeat under representative BLE/network-core traffic while checking frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher rates only after baseline stability.

## EasyDMA / DPPI note

After a simple synchronous SPIM path is validated, EasyDMA and supported GPIOTE/DPPI routing may reduce CPU load and trigger jitter. Keep buffer ownership explicit and validate the optimized path against the reference software-triggered implementation.

## 64-channel architecture

For 8 x ADS1299, multiple SPIM/DMA lanes or FPGA capture are preferred if tight cross-device skew matters. nRF5340 can then manage BLE, control, buffering and higher-level processing without disturbing sampling.

See `../DIRECT_USE.md` for the common Nordic flow.