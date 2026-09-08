# Renesas Synergy + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing Synergy files remain unchanged.

## SSP integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic ADS1299 callbacks to the Synergy SSP-generated SPI/GPIO/interrupt resources used by the project, then add `../_shared/renesas_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for initial hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Runtime architecture

The DRDY callback should only notify a high-priority acquisition task or deferred handler. That context reads one exact 27-byte ADS1299-8 frame, updates counters and queues the complete frame.

Recommended flow:

`DRDY -> event/semaphore -> 27-byte SPI read -> ring buffer -> USB/UART/Ethernet/application`

Logging, communications and signal processing should remain outside the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture and monitor frame alignment/read errors/queue overflow.
7. Add BIAS, lead-off and faster data rates only after stable baseline capture.

## Transfer-driver note

Once a simple synchronous SPI path is proven, use SSP transfer/DMA support where appropriate. Keep buffer ownership explicit between transfer completion and consumers so a frame cannot be overwritten while still in use.

## 64-channel note

For 8 x ADS1299, prefer multiple independent SPI/DMA paths or an FPGA capture front-end when tight synchronization matters. Synergy can then handle system control, buffering and transport.

See `../DIRECT_USE.md` for the common Renesas integration flow.