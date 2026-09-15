# MCXW236 MCUXpresso ADS1299 adapter

**Validation status: TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.**

This directory binds the existing MCXW236 `ads1299_platform_hal_t` boundary to NXP MCUXpresso SDK `LPSPI` and `GPIO` APIs. ADS1299 register commands, profiles, frame parsing and multi-device behavior remain in the shared ADS1299 core.

## What the board project owns

Configure pins and clocks in the normal MCUXpresso board/pin files, then fill one `ads1299_mcxw236_hal_context_t`. Do not edit the shared ADS1299 core for board wiring.

Required board-dependent values are:

- `lpspi` instance and its real source clock in Hz;
- SPI baud rate (start at or below 4 MHz during bring-up);
- GPIO port/pin tuples for CS, RESET, PWDN, START and DRDY;
- a microsecond delay callback.

The adapter rejects missing handles/clocks/delay callbacks, invalid or duplicated GPIOs, and a bring-up SPI rate above 4 MHz.

## SPI contract

`ads1299_mcxw236_hal_init()` configures the MCUXpresso LPSPI master for the ADS1299 bus contract: SPI Mode 1 (CPOL=0, CPHA=1), MSB first, 8-bit frames. CS is intentionally a GPIO controlled by the existing port layer, which also keeps the shared-SPI/independent-CS multi-ADS1299 architecture possible.

## Integration sequence

1. Start from an official MCXW236 MCUXpresso SDK project appropriate for the exact board you own.
2. Use Config Tools or the board pin files to route one LPSPI instance and five ADS1299 control GPIOs. Do not copy pin numbers from another MCXW236 board without checking its schematic.
3. Add `ads1299_mcxw236_hal.c`, the existing `../ads1299_port/*.c`, and the repository shared ADS1299 core sources to the project.
4. Fill `ads1299_mcxw236_hal_context_t`, call `ads1299_mcxw236_hal_init()`, then pass `ads1299_mcxw236_make_hal()` to `ads1299_mcu_port_init()`.
5. Use the existing model examples for probe, internal-test, input-short, 250-SPS EEG and streaming flows.

This file deliberately does not claim a reference-board pinout or successful MCUXpresso build until those are verified from the matching official board resources/toolchain.
