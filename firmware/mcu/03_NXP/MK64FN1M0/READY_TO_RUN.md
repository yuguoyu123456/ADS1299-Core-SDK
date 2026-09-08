# MK64FN1M0 + ADS1299 Ready-to-Run Guide

This guide is additive only. It does not remove or replace existing MK64FN1M0 support files.

## Recommended setup

- Reuse the existing NXP/Kinetis board and ADS1299 port layer in this directory.
- Add the shared helper:
  - `../_shared/nxp_ads1299_ready_app.c`
  - `../_shared/nxp_ads1299_ready_app.h`
- Add the common ADS1299 driver and `ads1299_profiles.c` to the project.
- Configure DSPI/SPI as master, CPOL=0, CPHA=1 (SPI Mode 1), MSB first.
- Start with a conservative 2–4 MHz SPI clock during first board validation.
- Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input.

## Runtime pattern

Use the DRDY interrupt only to signal that one new sample frame is ready. Do not perform UART/USB logging or signal processing in the GPIO ISR. In a high-priority acquisition context, read exactly one 27-byte ADS1299-8 frame, validate the frame header/status, increment the sequence number and enqueue the frame for downstream transport.

Because MK64FN1M0 has less memory and throughput headroom than i.MX RT devices, keep the acquisition path compact and use fixed-size ring buffers rather than large dynamic allocations.

## Validation order

1. Reset ADS1299 and read the ID register.
2. Apply `NXP_ADS1299_PROFILE_INTERNAL_TEST`.
3. Confirm stable generated test waveform frames.
4. Apply `NXP_ADS1299_PROFILE_INPUT_SHORT` and inspect baseline noise.
5. Apply `NXP_ADS1299_PROFILE_EEG_250` for initial electrode testing.
6. Add lead-off/BIAS only after frame alignment and transport stability are confirmed.

## DMA guidance

DSPI DMA can reduce CPU load, but first establish a reliable polling/interrupt implementation. When DMA is enabled, use statically allocated buffers and ensure no transport task overwrites a buffer before the transfer completes.

## 64-channel note

A single MK64FN1M0 should not be assumed to provide the best deterministic capture path for eight ADS1299 devices. For 64 channels, prefer an FPGA or multiple dedicated SPI acquisition engines and use the Kinetis MCU for control and lower-rate transport duties where appropriate.

See `../DIRECT_USE.md` for the shared NXP bring-up sequence.