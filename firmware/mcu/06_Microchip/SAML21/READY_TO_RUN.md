# SAML21 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing SAML21 files remain unchanged.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board support. Bind the generic ADS1299 callbacks to SERCOM/SPI, GPIO and EIC resources generated for the target project, then add the shared Microchip ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- SERCOM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 1–2 MHz if the intended low-power clock setup is conservative; increase only after reliable capture is proven.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge EIC input.

## Low-power acquisition pattern

Use the DRDY interrupt to wake/notify the acquisition context. That context reads one exact 27-byte ADS1299-8 frame and queues it before any optional return to sleep.

Recommended flow:

`DRDY -> wake/notify -> 27-byte SPI read -> static ring buffer -> transport/application -> optional sleep`

Do not perform logging or heavy processing in the interrupt callback.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Test sustained capture with the final sleep/wake policy enabled.
7. Monitor read errors and buffer overflow.
8. Add BIAS, lead-off and faster rates only after baseline stability.

## Low-power timing note

Measure wake-to-SPI latency and confirm that the selected sleep mode preserves the interrupt and clock resources needed to service every DRDY event.

## 64-channel note

For eight ADS1299 devices, use FPGA or a higher-throughput multi-SPI front-end when tight synchronization is required. SAML21 is best used as a low-power control/monitoring processor in such systems.

See `../DIRECT_USE.md` for the common Microchip flow.