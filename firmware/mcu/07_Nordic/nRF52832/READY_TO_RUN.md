# nRF52832 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing nRF52832 files remain unchanged.

## SDK integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks either to nRF Connect SDK/Zephyr SPI/GPIO APIs or, for legacy projects, to nRF5 SDK SPIM/GPIOTE resources. Add `../_shared/nordic_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPIM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO/GPIOTE input.

## Acquisition versus BLE

Keep the DRDY callback short. It should only signal a high-priority acquisition context. That context reads exactly one 27-byte ADS1299-8 frame and pushes it into a fixed-size ring buffer.

Recommended flow:

`DRDY -> event/semaphore -> SPIM 27-byte read -> ring buffer -> BLE/UART/application`

Do not perform BLE notifications, printf or signal processing in the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Run a sustained capture with BLE idle, then repeat under BLE traffic.
7. Monitor read errors and queue overflow.
8. Add BIAS, lead-off and faster rates only after stable baseline capture.

## EasyDMA note

After a synchronous transfer path is proven, SPIM EasyDMA can reduce CPU load. Use static RAM buffers and explicit ownership between the transfer completion event and the consumer.

## 64-channel note

nRF52832 is better used as a control/BLE endpoint than as the sole tight-skew capture engine for eight ADS1299 devices. For 64 channels, prefer FPGA or multiple deterministic SPI paths.

See `../DIRECT_USE.md` for the common Nordic flow.