# RA8T1 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing RA8T1 support remains unchanged.

## FSP integration

Reuse this directory's current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to Renesas FSP SPI/GPIO/external IRQ resources, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Deterministic sampling path

Keep DRDY handling minimal and predictable. The IRQ callback should notify a high-priority acquisition context, which performs one exact 27-byte ADS1299-8 read and immediately stores the complete frame in a ring buffer.

Control loops, motor/industrial workloads, logging and transport must be scheduled so they cannot block ADS1299 sampling.

## Validation order

1. Hardware reset and device-ID verification.
2. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
3. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
4. `RENESAS_ADS1299_PROFILE_EEG_250`.
5. Sustained capture under representative real-time application load.
6. Monitor frame/header alignment, read errors and buffer overflow.
7. Add BIAS, lead-off and higher rates only after the acquisition path is proven.

## DMA/cache note

If DMAC is used for SPI, select DMA-accessible memory and perform the cache maintenance required by the selected RA8 memory configuration. Use explicit ping-pong/ring-buffer ownership.

## 64-channel note

For eight ADS1299 devices, multiple independent SPI/DMA paths or FPGA capture are preferred for tight synchronization. RA8T1 can then handle deterministic control, aggregation and downstream processing without compromising sample timing.

See `../DIRECT_USE.md` for the common Renesas flow.