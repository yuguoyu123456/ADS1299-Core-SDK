# Reference pin map

Board: **Select an official STM32F303 evaluation board**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | SDK_SCK | SPI clock | 3.3 V digital |
| MOSI / DIN | SDK_MOSI | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | SDK_MISO | ADS1299 to MCU | 3.3 V digital |
| CS | SDK_CS | active-low chip select | 3.3 V digital |
| DRDY | SDK_DRDY | active-low data ready | 3.3 V digital |
| RESET | SDK_RESET | active-low reset | 3.3 V digital |
| PWDN | SDK_PWDN | active-low power-down | 3.3 V digital |
| START | SDK_START | conversion control | 3.3 V digital |
| stream TX | SDK_TX | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.

## Current reference-board assignment

For the standardized beginner path, the preferred reference board is now
**NUCLEO-F303RE (MB1136, STM32F303RET6)**. The generic `SDK_*` table above is
retained for backward compatibility with custom-board users.

| Signal | NUCLEO-F303RE pin | Arduino header | Notes |
|---|---:|---:|---|
| SCK | PA5 | D13 | SPI1_SCK |
| MOSI / DIN | PA7 | D11 | SPI1_MOSI |
| MISO / DOUT | PA6 | D12 | SPI1_MISO |
| CS | PB6 | D10 | GPIO output, active-low |
| DRDY | PA10 | D2 | GPIO/EXTI input, active-low |
| RESET | PB5 | D4 | GPIO output, active-low |
| PWDN | PB4 | D5 | GPIO output, active-low |
| START | PB10 | D6 | GPIO output |
| stream TX | PA2 | D1 | USART2_TX / optional host stream |

Use `board_config.h` as the single repository-owned board configuration point.
Configure SPI1 in STM32CubeMX as master/full-duplex, 8-bit, MSB-first, CPOL=0
and CPHA=1 (STM32 HAL `2Edge`), i.e. ADS1299 SPI Mode 1. Give the five control
signals the GPIO labels documented in `README.md` so generated `main.h` macros
match the board configuration header.

When using another STM32F303 board, change CubeMX pin/peripheral routing and
`board_config.h` only. Shared ADS1299 register/model/profile source files are
not board configuration files and should not be edited for normal bring-up.
