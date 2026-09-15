# MCXW716 MCUXpresso ADS1299 adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

This directory binds the existing MCXW716 ADS1299 platform port to MCUXpresso SDK LPSPI and GPIO APIs. It deliberately does not duplicate ADS1299 register, profile, frame, or command logic from `firmware/core_driver/ads1299/`.

## Integration

Add these files to an MCUXpresso SDK application for the selected MCXW716 board:

- `mcux_adapter/ads1299_mcxw716_hal.c`
- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- the shared ADS1299 core sources required by the application

Enable the MCUXpresso LPSPI and GPIO drivers (`fsl_lpspi.h`, `fsl_gpio.h`). Board clock/pin mux initialization remains in the vendor-generated board project.

Populate exactly one `ads1299_mcxw716_hal_context_t` with the board's LPSPI instance/source clock and GPIO coordinates for CS, RESET, PWDN, START and DRDY, plus a microsecond delay callback. Then call `ads1299_mcxw716_hal_init()` and pass `ads1299_mcxw716_make_hal()` to the existing MCXW716 port initialization path.

The adapter configures ADS1299 SPI as Mode 1 (`CPOL=0`, `CPHA=1`), MSB-first, 8-bit frames. The initial bring-up baud is intentionally rejected above 4 MHz. CS is a software GPIO; the LPSPI transfer does not substitute a hardware PCS for the ADS1299 port's CS ownership.

## Pin policy

No reference-board GPIO numbers are guessed here. Select and verify the pins against the exact MCXW716 board schematic and MCUXpresso pin-mux configuration. All five ADS1299 control/data-ready GPIOs must be valid and distinct. A different MCXW716 board should require changes only to board pin/clock configuration and the context values, not shared ADS1299 core files.

## Validation boundary

This adapter is source integration only. Repository CI or host tests do not prove that an MCXW716 MCUXpresso application has compiled, flashed, or communicated with ADS1299 hardware. Upgrade the status to **BUILD-VERIFIED** only after compiling the documented reference-board project with the real MCUXpresso SDK/toolchain, and to **BOARD-VERIFIED** only after physical hardware evidence exists.
