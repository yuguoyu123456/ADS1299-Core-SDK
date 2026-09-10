# Reference pin map

Board: **Select an official STM32H563 evaluation board**

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

## Standardized beginner reference mapping

For the maintained STM32H563 path, use **NUCLEO-H563ZI (MB1404, STM32H563ZIT6)** and `board_config.h` as the single obvious repository-owned board configuration point.

| ADS1299 / host signal | STM32H563 pin | Reference-board role |
|---|---:|---|
| SCLK | PA5 | Zio D13 / SPI1_SCK |
| DOUT / MISO | PG9 | Zio D12 / SPI1_MISO |
| DIN / MOSI | PB5 | Zio D11 / SPI1_MOSI |
| CS | PD14 | Zio D10 |
| DRDY | PD15 | Zio D9, active low |
| RESET | PF3 | Zio D8, active low |
| PWDN | PG2 | Zio D49, active low |
| START | PG3 | Zio D50 |
| host TX | PD8 | USART3_TX / ST-LINK VCP |
| host RX | PD9 | USART3_RX / ST-LINK VCP |

Configure SPI1 as **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB first**. The PA5/PG9/PB5 SPI1 route is preferred for the beginner reference path because ST documents it as usable on NUCLEO-H563ZI without making bring-up depend on the alternate PA7 MOSI solder-bridge change.

The historical `SDK_*` table above remains for compatibility with generic/custom projects. New students should start with `board_config.h` and the standardized mapping in this section instead of selecting pins ad hoc.
