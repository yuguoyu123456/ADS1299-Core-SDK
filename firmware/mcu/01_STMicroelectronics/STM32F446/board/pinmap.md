# Reference pin map

Board: **NUCLEO-F446RE (MB1136, STM32F446RET6)**

The original generic `SDK_*` intent is preserved below as a portability note, but the reference-board-first path now provides a concrete starter mapping in `board_config.h`.

## Concrete reference mapping

| ADS1299 signal | STM32F446RE pin | Nucleo / Arduino label | Function | Level |
|---|---|---|---|---|
| SCK | PA5 | D13 | SPI1_SCK | 3.3 V digital |
| MOSI / DIN | PA7 | D11 | SPI1_MOSI | 3.3 V digital |
| MISO / DOUT | PA6 | D12 | SPI1_MISO | 3.3 V digital |
| CS | PB6 | D10 | software chip select, active low | 3.3 V digital |
| DRDY | PC7 | D9 | data ready, active low | 3.3 V digital |
| RESET | PA9 | D8 | reset, active low | 3.3 V digital |
| PWDN | PA8 | D7 | power-down, active low | 3.3 V digital |
| START | PB10 | D6 | conversion control | 3.3 V digital |
| stream TX | PA2 | ST-LINK VCP | USART2_TX | 3.3 V digital |
| stream RX | PA3 | ST-LINK VCP | USART2_RX | 3.3 V digital |

SPI1 must be configured as ADS1299-required **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first** with software-controlled CS.

On the default MB1136 solder-bridge configuration, PA2/PA3 are routed to the ST-LINK virtual COM port. If PA2/PA3 are intentionally rerouted to the Arduino/Morpho connectors, follow the board user manual and solder-bridge table; do not assume VCP and header routing are simultaneously available.

## Portability placeholders retained from the original scaffold

| Signal | Generic name | Function | Level |
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

For a different STM32F446 board, map those logical signals in `board_config.h` and the vendor-generated HAL project. The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm header routing, boot straps, solder bridges and debugger conflicts against the exact board schematic before connecting hardware.

Do not change shared ADS1299 core files merely to accommodate another pin map.
