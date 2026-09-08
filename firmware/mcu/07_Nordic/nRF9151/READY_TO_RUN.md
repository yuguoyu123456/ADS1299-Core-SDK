# nRF9151 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing nRF9151 files remain unchanged.

## nRF Connect SDK integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to nRF Connect SDK/Zephyr SPIM and GPIO resources selected through devicetree, then add `../_shared/nordic_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPIM master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for first hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt input.

## Acquisition versus cellular networking

The DRDY callback should only notify a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates sequence/error counters and stores the complete frame into a ring buffer.

Recommended flow:

`DRDY -> acquisition notification -> 27-byte SPIM read -> ring buffer -> modem/socket/cloud/application task`

Do not perform modem, socket, TLS, cloud, logging or packetization work in the DRDY callback.

## Validation sequence

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Long capture with modem/network activity minimized.
7. Repeat under representative cellular traffic and check frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and faster sample rates only after stable baseline operation.

## EasyDMA / power-management note

After a simple SPIM path is proven, EasyDMA can reduce CPU occupancy. Cellular power-management states must not prevent timely DRDY servicing or invalidate the SPI/GPIO assumptions; test the final modem/power policy during sustained acquisition.

## 64-channel architecture

For eight ADS1299 devices, an FPGA or multiple deterministic SPI/DMA paths are preferred when tight inter-device skew matters. nRF9151 is best used for control and cellular uplink after deterministic capture is established.

See `../DIRECT_USE.md` for the common Nordic integration flow.