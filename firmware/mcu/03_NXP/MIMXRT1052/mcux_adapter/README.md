# MIMXRT1052 MCUXpresso ADS1299 adapter

Status: **TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED**.

This directory binds the existing MIMXRT1052 `ads1299_port` abstraction to MCUXpresso SDK LPSPI and GPIO APIs. ADS1299 register setup, profiles, frame parsing, self-test and multi-device behavior remain in shared repository layers.

## Board/application responsibilities

Create one `ads1299_mimxrt1052_hal_context_t` in the board/application layer and provide the selected `LPSPI_Type *`, its real source clock, ADS1299 SPI baud, GPIO controller/pin pairs for CS, RESET, PWDN, START and DRDY, and a microsecond delay callback.

Pin mux, pad configuration, peripheral clocks and GPIO direction setup remain in the MCUXpresso board project. Do not guess EVK pins: use the exact board schematic and the project's generated/configured pin mux. CS, RESET, PWDN and START are outputs; DRDY is an input.

## Integration

1. Add `ads1299_mimxrt1052_hal.c`, this model's existing `ads1299_port` sources, and the shared ADS1299 sources required by the selected example.
2. Initialize board clocks, IOMUXC and GPIO directions.
3. Fill `ads1299_mimxrt1052_hal_context_t` and call `ads1299_mimxrt1052_hal_init()`.
4. Call `ads1299_mimxrt1052_make_hal()` and pass that HAL to `ads1299_mcu_port_init()`.
5. Run the existing progressive model examples in order: probe/ID, internal test, input short, 250-SPS EEG, then streaming.

The adapter configures LPSPI for ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB-first, 8-bit transfers. CS is controlled by the repository GPIO callback rather than delegated to hardware PCS.

## Diagnostics

`ads1299_mimxrt1052_hal_init()` returns `-1` for missing LPSPI/source clock/delay or invalid bring-up baud, `-2` for invalid GPIO, `-3` for duplicate ADS1299 signal GPIOs, and `-4` when `LPSPI_MasterInit()` fails. SPI transfer failures propagate as `-1` through the existing HAL callback.

The adapter deliberately limits initial SPI configuration to 4 MHz. This is a conservative bring-up guard, not a claim about the maximum rate of every finished design.

## Validation boundary

The implementation follows the MCUXpresso i.MX RT LPSPI/GPIO API shape (`GPIO_Type * + pin`). It does **not** prove that a particular MIMXRT1052 EVK project compiles, that a chosen pin map is correct, or that ADS1299 hardware has been exercised. Promote to BUILD-VERIFIED only after a documented reference project compiles successfully and to BOARD-VERIFIED only after physical-board evidence exists.
