# Reference pin map

Board: **Select an official STM32F072 evaluation board**

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

## Concrete first reference: NUCLEO-F072RB

For the standardized beginner path, use **NUCLEO-F072RB (MB1136)** first. The
repository reference mapping is:

| Signal | STM32F072RB pin | Arduino header | Role |
|---|---|---|---|
| SCK | PA5 | D13 | SPI1_SCK |
| MISO / DOUT | PA6 | D12 | SPI1_MISO |
| MOSI / DIN | PA7 | D11 | SPI1_MOSI |
| CS | PB6 | D10 | software-controlled active-low CS |
| START | PB10 | D6 | conversion control GPIO |
| PWDN | PB4 | D5 | active-low power-down GPIO |
| RESET | PB5 | D4 | active-low reset GPIO |
| DRDY | PA10 | D2 | active-low data-ready input |
| stream TX | PA2 | D1 | optional USART2_TX / ST-LINK VCP path |

Use `board_config.h` as the single repository-owned board configuration point.
In CubeMX label the control pins `ADS1299_CS`, `ADS1299_START`, `ADS1299_PWDN`,
`ADS1299_RESET`, and `ADS1299_DRDY`; the generated `main.h` symbols then match
the aliases in `board_config.h`.

SPI1 must be configured as master, full duplex, 8-bit, MSB-first, CPOL Low,
CPHA 2Edge (SPI Mode 1). If another STM32F072 board is used, change the CubeMX
pin/peripheral assignment and board aliases only; do not edit shared ADS1299
register/profile logic.

This mapping is a documented reference integration, not a claim of physical
ADS1299 board validation.
