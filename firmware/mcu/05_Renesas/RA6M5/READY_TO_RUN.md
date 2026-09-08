# RA6M5 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing RA6M5 files remain unchanged.

## FSP integration

Reuse this directory's current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to Renesas FSP SPI/GPIO/external IRQ resources, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for board bring-up.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition architecture

Recommended pipeline:

`DRDY -> short ISR/callback -> high-priority acquisition task -> one exact 27-byte ADS1299-8 frame -> ring buffer -> Ethernet/USB/UART/application`

Keep communications, logging and signal processing outside the DRDY callback. Complete frames should be queued before any downstream processing begins.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform long continuous capture and monitor frame/header alignment, read errors and queue overflow.
7. Stress the final transport path.
8. Add BIAS, lead-off and higher data rates only after the baseline path is stable.

## DMA and buffering

RA6M5 has enough throughput for a strong MCU-side implementation. After a known-good polling/interrupt baseline is established, use DMAC/DTC or transfer drivers with ping-pong buffers. Make buffer ownership explicit between the SPI completion path and the consumer task.

## 8 x ADS1299 / 64 channels

RA6M5 can participate in multi-device acquisition, but tight cross-device skew is best handled with multiple independent SPI/DMA paths or an FPGA capture front-end. Use shared START/clock strategy where required by the hardware design and keep each complete device frame independently identifiable before packet aggregation.

See `../DIRECT_USE.md` for the common Renesas integration flow.