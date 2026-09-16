# LPC54628 MCUXpresso hardware adapter

Validation: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

`ads1299_lpc54628_mcuxpresso.[ch]` binds the existing portable LPC54628 `ads1299_platform_hal_t` contract to MCUXpresso SDK SPI/GPIO APIs. It deliberately does not invent a board pin mux, clock tree, SDK project, startup file, or linker script.

## Consuming project prerequisites

Use an MCUXpresso SDK project for the exact LPC54628 board/device. In the vendor project, initialize clocks and IOCON/pin mux first, then populate one `ads1299_lpc54628_hw_t` with the selected `SPI_Type *`, SPI source-clock frequency, baud rate, five GPIO port/pin descriptors, and a real microsecond delay callback. Call `ads1299_lpc54628_mcuxpresso_init()` to obtain the `ads1299_platform_hal_t`, then pass that HAL to the existing `ads1299_mcu_port_init()` flow used by the examples.

The adapter configures SPI as ADS1299-required Mode 1 (CPOL=0, CPHA=1), MSB first, and rejects bring-up baud rates above 4 MHz. CS remains a software GPIO; the SPI transfer does not depend on a hardware slave-select assignment.

## Error stages

- `-1`: missing hardware context, SPI instance, HAL destination, or delay callback.
- `-2`: one of CS/RESET/PWDN/START/DRDY lacks a GPIO controller.
- `-3`: invalid source clock/baud or bring-up baud above 4 MHz.
- `-4`: MCUXpresso `SPI_MasterInit()` failed.

The exact reference-board pin map remains unresolved in this template. Do not copy placeholder `SDK_*` names into hardware: select pins from the exact board schematic and configure their IOCON functions in the consuming MCUXpresso project. Once a concrete official LPC54628 board project is imported and compiled with these files, the validation status can advance to BUILD-VERIFIED; only physical ADS1299 execution can justify BOARD-VERIFIED.
