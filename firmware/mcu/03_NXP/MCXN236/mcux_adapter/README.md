# MCXN236 MCUXpresso ADS1299 adapter

This directory binds the existing MCXN236 `ads1299_platform_hal_t` boundary to MCUXpresso SDK LPSPI and GPIO APIs. ADS1299 register, command, frame, profile, and multi-device behavior remains in the shared repository core.

## Integration boundary

Add `ads1299_mcxn236_hal.c` to the MCUXpresso application together with this model's existing `ads1299_port/*.c` files and the shared ADS1299 core sources. Add include paths for this directory, `../ads1299_port`, and `firmware/core_driver/ads1299`.

Board-generated MCUXpresso code remains responsible for clock setup, pin mux, GPIO directions, the selected LPSPI instance, and the actual ADS1299 wiring. Do not edit shared ADS1299 core files for board pins.

Create one `ads1299_mcxn236_context_t`, populate `spi`, `gpio`, the five `ports[]`/`pins[]` entries in `ADS1299_PIN_CS ... ADS1299_PIN_DRDY` order, `source_hz`, `spi_hz`, `poll_limit`, and a microsecond delay callback. Then call `ads1299_mcxn236_make_hal()` and pass the returned HAL to the existing MCXN236 port initialization.

The adapter configures LPSPI as ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, 8-bit frames. Initial bring-up is deliberately rejected above 4 MHz. CS is software GPIO so a shared SPI bus can use independent CS lines for multiple ADS1299 devices.

## Validation status

**TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.** The implementation uses MCUXpresso SDK `fsl_lpspi.h` and `fsl_gpio.h` APIs, but this repository does not yet claim a successful MCXN236 reference-board SDK build or physical-board run. Keep that distinction when updating validation documentation.
