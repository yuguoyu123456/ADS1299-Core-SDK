# MIMXRT1021 MCUXpresso ADS1299 adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

This directory binds the existing MIMXRT1021 ADS1299 platform port to MCUXpresso SDK LPSPI and GPIO APIs. It deliberately does not duplicate ADS1299 register, profile, frame, or command logic from `firmware/core_driver/ads1299/`.

## Integration

Add these files to an MCUXpresso SDK application for the selected MIMXRT1021 board:

- `mcux_adapter/ads1299_mimxrt1021_hal.c`
- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- the shared ADS1299 core sources required by the application

Enable the MCUXpresso LPSPI and GPIO drivers (`fsl_lpspi.h`, `fsl_gpio.h`). Keep clock-tree, IOMUXC and pad configuration in the vendor-generated board project; this adapter does not invent startup, linker, clock or pin-mux code.

Populate one `ads1299_mimxrt1021_hal_context_t` with the board's LPSPI instance/source clock and GPIO controller/pin pairs for CS, RESET, PWDN, START and DRDY, plus a microsecond delay callback. Call `ads1299_mimxrt1021_hal_init()`, then pass `ads1299_mimxrt1021_make_hal()` to the existing MIMXRT1021 port initialization path.

The adapter configures ADS1299 SPI as Mode 1 (`CPOL=0`, `CPHA=1`), MSB-first, 8-bit frames. Initial bring-up baud is rejected above 4 MHz. CS remains owned by the existing ADS1299 software-GPIO port rather than by LPSPI PCS.

## i.MX RT GPIO API note

The i.MX RT MCUXpresso GPIO API identifies a pin by `GPIO_Type *` plus pin number. The context therefore intentionally has no separate GPIO `port` field. Keep MCX-family adapters with their own GPIO calling convention; do not force the two families into an incorrect common vendor API shape.

## Pin policy

No reference-board GPIO numbers are guessed here. Select and verify pins against the exact MIMXRT1021 board schematic and MCUXpresso IOMUXC configuration. All five ADS1299 control/data-ready GPIOs must be valid and distinct. Moving to another MIMXRT1021 board should require board clock/IOMUXC changes and context-value changes only, not edits to shared ADS1299 core files.

## Validation boundary

This adapter is source integration only. Repository CI or host tests do not prove that a MIMXRT1021 MCUXpresso application has compiled, flashed, or communicated with ADS1299 hardware. Upgrade the status to **BUILD-VERIFIED** only after compiling the documented reference-board project with the real MCUXpresso SDK/toolchain, and to **BOARD-VERIFIED** only after physical hardware evidence exists.
