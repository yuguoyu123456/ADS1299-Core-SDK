# MCXN947 MCUXpresso hardware adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

This directory binds the existing MCXN947 ADS1299 platform port to MCUXpresso SDK `LPSPI` and `GPIO`. It deliberately does not duplicate ADS1299 register, command, profile, frame, or multi-device logic from the shared core.

## Board integration contract

Board/generated MCUXpresso code must configure pin muxes, GPIO directions, clocks, the chosen LPSPI instance, and a microsecond delay source. Then populate one `ads1299_mcxn947_context_t` and call `ads1299_mcxn947_make_hal()` before the existing MCXN947 port initialization.

The five `ports[]` / `pins[]` entries use `ads1299_platform_pin_t` order: CS, RESET, PWDN, START, DRDY. CS/RESET/PWDN/START are outputs; DRDY is an input. Keep ADS1299 CS as software GPIO; do not rely on automatic PCS timing.

The adapter configures LPSPI as SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, 8 bits/frame. Bring-up is intentionally rejected above 4 MHz. Configuration also fails for missing SPI/GPIO handles, clocks, delay callback, poll limit, invalid pin numbers, or duplicate control/DRDY GPIO assignments.

## What a board project must supply

1. MCUXpresso SDK headers/drivers including `fsl_lpspi.h` and `fsl_gpio.h`.
2. A configured `LPSPI_Type *` and its actual source clock frequency.
3. Pin-muxed SCK/SDO/SDI plus five board-selected ADS1299 GPIOs.
4. GPIO output initialization for CS/RESET/PWDN/START and input initialization for DRDY.
5. A bounded microsecond delay callback.
6. The existing `ads1299_port` and shared ADS1299 core sources required by the MCXN947 example/project.

No reference-board pin assignment is claimed here until it is verified against an official board schematic/SDK project. A student should change board/generated configuration and this context only; shared ADS1299 core files are not a board-configuration surface.
