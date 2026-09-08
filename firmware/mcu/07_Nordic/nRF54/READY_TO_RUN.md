# nRF54 Series + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing nRF54 files remain unchanged.

## nRF Connect SDK integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to the SPIM/GPIO resources exposed by the selected nRF54 target through nRF Connect SDK/Zephyr devicetree, then add the shared Nordic ready-app helper and common ADS1299 core/profile sources.

Because this directory represents a series rather than one fixed package/board, keep pinmux, SPIM instance and DMA/event-routing choices board-specific.

## Peripheral setup

- SPIM master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin at approximately 2–4 MHz for board bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt input.

## Acquisition path

Use DRDY only as the sampling trigger. The GPIO callback should signal a high-priority acquisition context, which performs one exact 27-byte ADS1299-8 transfer, updates sequence/error counters and places the complete frame into a ring buffer.

Keep BLE/Thread/application logging and DSP downstream of capture.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Long capture with radio/application load minimized.
7. Repeat with representative radio and application traffic while monitoring read errors and queue overflow.
8. Add BIAS, lead-off and faster rates only after stable baseline capture.

## EasyDMA / DPPI guidance

Where the selected nRF54 device exposes SPIM EasyDMA and event routing suitable for the design, use them only after validating a simple reference path. Keep DMA buffers in suitable RAM and maintain explicit ownership between transfer completion and consumers.

## 64-channel architecture

For eight ADS1299 devices, use multiple deterministic SPI/DMA paths or an FPGA front-end when tight synchronization is required. nRF54-class devices can then focus on control, wireless transport, buffering and signal processing according to the chosen device resources.

See `../DIRECT_USE.md` for the common Nordic integration flow.