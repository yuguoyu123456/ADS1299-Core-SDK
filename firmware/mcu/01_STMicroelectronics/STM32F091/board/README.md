# STM32F091 board configuration

## Reference board

The first reproducible target for this model is **NUCLEO-F091RC** (MB1136,
STM32F091RCT6). Use the official STM32CubeF0 package and STM32CubeIDE/CubeMX
project generation for startup code, clocks, HAL drivers and linker files.

This directory is the board-specific part of the ADS1299 integration. A normal
board change must not require edits to `firmware/core_driver/ads1299/`.

## What a beginner edits

1. Configure the intended pins/peripherals in STM32CubeMX.
2. Keep the GPIO labels listed below so generated `main.h` names remain stable.
3. If a different SPI/UART instance is chosen, edit only `board_config.h`.

Required CubeMX GPIO labels:

- `ADS1299_CS`
- `ADS1299_RESET`
- `ADS1299_PWDN`
- `ADS1299_START`
- `ADS1299_DRDY`

Default repository handles are `hspi1` and `huart2`.

## Reference wiring

| ADS1299 signal | STM32F091RC | Nucleo header | Direction |
|---|---|---|---|
| SCLK | PA5 / SPI1_SCK | D13 | MCU -> ADS1299 |
| DOUT / MISO | PA6 / SPI1_MISO | D12 | ADS1299 -> MCU |
| DIN / MOSI | PA7 / SPI1_MOSI | D11 | MCU -> ADS1299 |
| CS | PB6 | D10 | MCU -> ADS1299 |
| START | PB10 | D6 | MCU -> ADS1299 |
| PWDN | PB4 | D5 | MCU -> ADS1299 |
| RESET | PB5 | D4 | MCU -> ADS1299 |
| DRDY | PA10 | D2 | ADS1299 -> MCU |
| optional stream TX | PA2 / USART2_TX | D1 | MCU -> host |
| digital ground | GND | GND | common reference |

Verify the ADS1299 digital supply level, the exact daughterboard power scheme
and common ground before wiring. This table covers the MCU digital interface;
it is not an analog-front-end power or patient-electrode wiring guide.

## SPI settings

Configure SPI1 as:

- master, full duplex;
- 8-bit data size;
- MSB first;
- CPOL = Low;
- CPHA = 2 Edge;
- software-managed chip select; and
- an SCLK rate that satisfies the ADS1299 timing limits for the selected clock.

CPOL Low + CPHA 2 Edge is SPI Mode 1. `CS` is driven by the repository port
layer rather than hardware NSS.

## Changing to another STM32F091 board

Keep the shared core and ADS1299 protocol code unchanged. Change CubeMX pin
assignment/peripheral instances, retain the generated GPIO labels when
possible, and update `board_config.h` handle macros if needed. If header pins or
solder bridges differ, update the local pin map documentation for that board.

## Validation status

**TEMPLATE / reference integration configuration.** The mapping is based on the
official NUCLEO-F091RC/MB1136 documentation. It does not claim that the current
repository project has been built in STM32CubeIDE or exercised on physical
NUCLEO-F091RC + ADS1299 hardware.
