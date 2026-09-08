# MSP430 + ADS1299 Ready-to-Run Guide

This guide adds practical integration notes while preserving all existing MSP430 files.

## Integration

Reuse the existing `ads1299_port/` layer and bind it to the MSP430 SPI/GPIO implementation used by the target device or board. Add the shared TI ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 1–2 MHz if the selected MSP430/device clock is modest; increase only after stable validation.
- CS, RESET, PWDN, START outputs.
- DRDY falling-edge interrupt input.

## Resource-conscious acquisition

MSP430 targets are generally more resource constrained than Cortex-M or C2000 devices. Keep the DRDY ISR minimal and read exactly one ADS1299-8 27-byte frame in a compact high-priority context. Use fixed-size static buffers; avoid dynamic allocation and floating-point conversion in the timing-critical path.

Recommended flow:

`DRDY -> flag -> SPI frame read -> fixed ring buffer -> UART/USB/other transport`

## Validation

1. Reset and verify ADS1299 ID.
2. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
3. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
4. Run `TI_ADS1299_PROFILE_EEG_250`.
5. Confirm long continuous capture without frame shifts.
6. Add optional BIAS/lead-off only after basic capture is stable.

## Throughput note

For raw EEG streaming, calculate the final serial/USB link budget before enabling higher rates. Keep raw integer samples in the MCU and move voltage conversion/advanced processing to a host when memory/CPU headroom is limited.

## 64-channel note

MSP430 should not be assumed to be the primary acquisition engine for eight ADS1299 devices. For 64 channels, use FPGA or a higher-throughput MCU/front-end and reserve MSP430 for supervisory/control roles if appropriate.

See `../DIRECT_USE.md` for the shared TI flow.