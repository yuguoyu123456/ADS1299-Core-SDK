# SAMD21 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing SAMD21 files remain unchanged.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to SERCOM/SPI, GPIO and EIC resources configured through Harmony 3 or the existing project layer, then add `../_shared/microchip_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- SERCOM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for first validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC input.

## Acquisition path

Keep the DRDY callback minimal. It should notify a high-priority acquisition context, which reads one exact 27-byte ADS1299-8 frame and stores the complete frame into a static ring buffer.

Do not perform USB, UART, logging or floating-point conversion inside the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Perform sustained capture and monitor read errors/queue overflow.
7. Add BIAS, lead-off and higher rates only after baseline stability.

## DMAC note

After a known-good non-DMA path is established, use DMAC with explicit buffer ownership if needed. Keep buffers static and sized so transport jitter cannot immediately cause loss.

## 64-channel note

For 8 x ADS1299, use FPGA or multiple deterministic SPI/DMA lanes if tight synchronization is required. SAMD21 is better suited to modest channel counts, control and moderate-rate transport.

See `../DIRECT_USE.md` for the common Microchip flow.