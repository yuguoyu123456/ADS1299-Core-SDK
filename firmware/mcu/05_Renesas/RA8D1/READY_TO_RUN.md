# RA8D1 + ADS1299 Ready-to-Run Guide

This file only adds practical integration guidance. Existing RA8D1 files remain intact.

## FSP integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to the Renesas FSP SPI/GPIO/external IRQ resources generated for the target project, then add `../_shared/renesas_ads1299_ready_app.c/.h` plus the common ADS1299 driver/profile sources.

## Peripheral setup

- SPI master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Use a conservative 2–4 MHz clock during initial validation.
- Configure CS, RESET, PWDN and START as outputs.
- Configure DRDY as a falling-edge external interrupt input.

## Deterministic acquisition

Use DRDY as the real-time trigger only. The IRQ callback should signal a high-priority acquisition context, which reads exactly one 27-byte ADS1299-8 frame, updates sequence/error counters and writes the frame into a DMA-safe ring buffer.

Display/UI, graphics, networking, logging and signal processing should remain downstream of the capture path.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform a sustained capture with final application load enabled.
7. Monitor read errors, header alignment and queue overflow.
8. Add BIAS, lead-off and higher sample rates only after stable baseline capture.

## DMA/cache note

For DMA-backed SPI on high-performance RA8 designs, place transfer buffers in DMA-accessible memory and perform the cache clean/invalidate operations required by the selected memory region. If unexplained corruption appears, compare against a simple non-DMA baseline before blaming ADS1299 timing.

## 64-channel architecture

RA8D1 has strong compute headroom, but for eight ADS1299 devices the cleanest low-skew design still uses multiple SPI/DMA lanes or an FPGA capture stage. RA8D1 can then handle buffering, visualization, DSP, network transport and system control without disturbing the sample timing path.

See `../DIRECT_USE.md` for the common Renesas flow.