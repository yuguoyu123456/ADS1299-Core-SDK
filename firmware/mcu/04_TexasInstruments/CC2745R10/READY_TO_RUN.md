# CC2745R10 + ADS1299 Ready-to-Run Guide

This file only adds integration guidance; existing CC2745R10 files are preserved.

## Project integration

Reuse the existing `ads1299_port/` implementation and board abstraction in this folder. Bind its callbacks to the target SimpleLink / TI Drivers SPI and GPIO instances, then add the shared TI ready-app and common ADS1299 profile/core sources.

## Peripheral setup

- SPI master, CPOL=0, CPHA=1 (Mode 1), MSB first.
- Use a conservative 2–4 MHz clock during initial validation.
- Configure CS/RESET/PWDN/START as outputs.
- Configure DRDY as a falling-edge interrupt source.

## Sampling architecture

The DRDY handler should only acknowledge the interrupt and notify a high-priority acquisition context. That context reads one complete ADS1299-8 frame (27 bytes), updates sequence/error counters and pushes the frame into a ring buffer.

BLE/2.4 GHz packetization, encryption, logging and application code should be downstream consumers rather than part of the sampling callback.

## Bring-up order

1. Hardware reset ADS1299.
2. Read and verify ID.
3. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `TI_ADS1299_PROFILE_EEG_250`.
6. Validate long continuous capture with no silent drops.
7. Add BIAS/lead-off and higher sample rates after the baseline path is stable.

## Wireless note

Radio scheduling must not determine ADS1299 service latency. Use a bounded queue and explicit overflow accounting so a slow wireless link produces visible backpressure rather than corrupting sample timing.

## 64-channel note

For 8 x ADS1299, prefer deterministic FPGA or parallel SPI capture. CC2745R10 can then focus on control, wireless transport and system management.

See `../DIRECT_USE.md` for the common TI integration flow.