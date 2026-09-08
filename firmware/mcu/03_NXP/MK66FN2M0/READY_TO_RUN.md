# MK66FN2M0 + ADS1299 Ready-to-Run Guide

This guide extends the existing MK66FN2M0 folder without deleting or replacing any existing files.

## Minimal direct-use setup

1. Keep the current Kinetis board/port files in this directory.
2. Add `../_shared/nxp_ads1299_ready_app.c/.h` and the common ADS1299 driver/profile sources.
3. Configure DSPI/SPI as master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
4. Start board bring-up with a conservative 2–4 MHz SPI clock.
5. Configure CS, RESET, PWDN and START as outputs; configure DRDY for falling-edge interrupt.

## Acquisition path

The DRDY ISR should only clear the GPIO interrupt and notify a high-priority acquisition context. That acquisition context reads exactly 27 bytes from ADS1299-8, checks frame alignment/status, increments the sample counter and stores the complete frame in a fixed-size ring buffer. USB/UART/Ethernet/storage code should consume frames independently and must never block the sampling path.

MK66FN2M0 provides more memory/peripheral headroom than K64-class devices, but deterministic acquisition should still be treated as a real-time path and separated from communication work.

## Validation sequence

- Reset and verify ADS1299 ID.
- Run `NXP_ADS1299_PROFILE_INTERNAL_TEST`.
- Run `NXP_ADS1299_PROFILE_INPUT_SHORT`.
- Run `NXP_ADS1299_PROFILE_EEG_250`.
- Verify long continuous captures for frame/header alignment and zero queue overflow.
- Add BIAS, lead-off and higher sample-rate profiles only after the basic stream is stable.

## DMA recommendation

After polling/interrupt transfers are proven, DSPI DMA can reduce CPU occupancy. Use static RX/TX buffers, explicit ownership between DMA and consumer code, and a queue-depth counter so dropped or overwritten frames are visible instead of silent.

## 8 x ADS1299 / 64 channels

For eight converters, use multiple SPI engines or an FPGA capture stage when tight synchronization matters. The MK66 can remain responsible for configuration, health monitoring, packet framing and transport if the acquisition bandwidth and latency are validated for the selected interface.

See `../DIRECT_USE.md` for the common NXP integration flow.