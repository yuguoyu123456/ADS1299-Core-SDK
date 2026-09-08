# RA2E2 + ADS1299 Ready-to-Run Guide

This file adds direct-use guidance only. Existing RA2E2 files remain intact.

## FSP integration

Reuse the current `ads1299_port/` and board files. Bind the generic ADS1299 callbacks to Renesas FSP SPI/GPIO/IRQ instances generated for the target project, then add the shared Renesas ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 1–2 MHz if the low-power clock configuration is conservative; increase only after stable validation.
- CS/RESET/PWDN/START as outputs.
- DRDY as a falling-edge external interrupt input.

## Runtime flow

Keep the external IRQ callback minimal. It should only signal a high-priority acquisition context, which reads exactly one 27-byte ADS1299-8 frame, updates the sample/error counters and writes the frame to a fixed-size ring buffer.

Do not perform UART logging, signal processing or blocking transport in the DRDY callback.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Validate sustained capture with no silent frame loss.
7. Add BIAS, lead-off and higher rates after baseline stability.

## Low-power note

RA2E2 is suited to low-power control, but sleep policy must not prevent timely DRDY service. Use a wake/notify pattern and keep the ADS1299 sampling path independent of long application tasks.

## 64-channel note

For eight ADS1299 devices, use FPGA or a higher-throughput multi-SPI capture engine if tight inter-device timing is required. RA2E2 can still provide configuration, monitoring and supervisory control.

See `../DIRECT_USE.md` for the shared Renesas integration flow.