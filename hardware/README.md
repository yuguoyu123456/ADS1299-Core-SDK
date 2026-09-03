# Hardware

This folder is the hardware entry point for ADS1299-Core systems.

## Sections

- `ads1299_core_module/` — module pinout, power, analog input, reference, BIAS and board-level validation.
- `multi_ads1299/` — 16/32/64-channel systems using the project-default multi-device SPI architecture with independent chip-select control.
- `accessories/` — electrode adapters, isolation, backplanes and related hardware.

## Recommended order

1. Bring up one ADS1299 first.
2. Verify ID, internal test and input-short noise.
3. Scale to 2 devices, then 4, then 8.
4. Use shared SCLK/MOSI/MISO with one CS per ADS1299 as the normal multi-device architecture.
5. Validate common clock / START synchronization and per-device DRDY behavior before declaring 32/64-channel support.

Electrical limits and timing must always be checked against the current Texas Instruments ADS1299 datasheet.
