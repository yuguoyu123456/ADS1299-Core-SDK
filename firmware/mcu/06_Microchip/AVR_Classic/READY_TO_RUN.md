# Classic AVR + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing classic AVR files remain unchanged.

## Integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic callbacks to the classic AVR SPI/GPIO/interrupt implementation used by the target device, then add the shared Microchip ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Select a conservative SPI clock compatible with the MCU clock; 0.5–2 MHz is a practical bring-up range for many classic AVR targets.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external/pin-change interrupt where supported.

## Resource-conscious acquisition

Classic AVR devices have limited RAM and CPU headroom. Keep the DRDY ISR short, read one exact 27-byte ADS1299-8 frame in the acquisition path and store it in a small fixed ring buffer.

Avoid dynamic allocation, floating-point conversion, printf and complex packet processing during acquisition.

## Validation order

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Confirm sustained frame alignment with the final serial transport enabled.
7. Add optional BIAS/lead-off only after stable baseline capture.

## Throughput note

Keep channel samples as raw 24-bit integers and convert to engineering units on the host. Calculate UART/USB/serial bandwidth before increasing the sample rate, and expose buffer overflow as a visible error.

## 64-channel note

Classic AVR should not be used as the primary synchronized capture engine for eight ADS1299 devices. Use FPGA or a substantially higher-throughput MCU/front-end for 64 channels.

See `../DIRECT_USE.md` for the common Microchip integration flow.