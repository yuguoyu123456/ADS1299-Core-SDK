# Reference pin map

Board: **Select an official STM32F429 evaluation board**

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

---

## Repository reference path: STM32F429I-DISC1 (MB1075)

The standardized reference board for the current SDK path is
**STM32F429I-DISC1 / MB1075 / STM32F429ZIT6**.  The single repository-owned
configuration point is `board_config.h`.

| ADS1299 signal | Starter MCU GPIO | Peripheral/function |
|---|---|---|
| SCLK | PA5 | SPI1_SCK |
| DOUT | PA6 | SPI1_MISO |
| DIN | PA7 | SPI1_MOSI |
| CS | PB0 | software GPIO, active low |
| DRDY | PB1 | GPIO input / optional EXTI, active low |
| RESET | PB11 | GPIO output, active low |
| PWDN | PB12 | GPIO output, active low |
| START | PB13 | GPIO output |
| stream TX | PA9 | USART1_TX / ST-LINK VCP path on DISC1 |
| stream RX | PA10 | USART1_RX / optional |

Configure SPI as Mode 1 (CPOL=0, CPHA=1), 8-bit and MSB-first.  Use software
NSS so each ADS1299 can retain an independent CS on a shared SPI bus.

STM32F429I-DISC1 contains onboard LCD, SDRAM, MEMS and ST-LINK/VCP circuitry.
Before connecting the ADS1299, compare this starter routing with the exact
MB1075 revision and the generated CubeMX pinout.  If a required onboard
function conflicts, choose a valid alternate GPIO/peripheral route and update
only CubeMX plus `board_config.h`; do not change the shared ADS1299 core.

The STM32F429I-DISC1 VCP path is USART1 PA9/PA10 when its solder-bridge routing
is connected.  This reference mapping intentionally avoids treating USART2 as
the default Discovery-board VCP.
