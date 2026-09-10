# Reference pin map

Board: **Select an official STM32H533 evaluation board**

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

## Standardized reference mapping added for the SDK

The current reference-board-first path is **NUCLEO-H533RE (MB1814, STM32H533RET6)**. The legacy `SDK_*` table above is preserved for compatibility with earlier generic integrations; new beginner work should use `board_config.h` and the mapping below.

| ADS1299 signal | STM32H533 pin | Nucleo role |
|---|---:|---:|
| SCK | PA5 | Arduino D13 / SPI1_SCK |
| MOSI / DIN | PA7 | Arduino D11 / SPI1_MOSI |
| MISO / DOUT | PA6 | Arduino D12 / SPI1_MISO |
| CS | PC9 | Arduino D10 |
| DRDY | PC6 | Arduino D9, active low |
| RESET | PC7 | Arduino D8, active low |
| PWDN | PA8 | Arduino D7, active low |
| START | PB10 | Arduino D6 |
| stream TX | PA2 | USART2_TX / STLINK-V3EC VCP |
| stream RX | PA3 | USART2_RX / STLINK-V3EC VCP |

SPI must be configured as **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first**. `board_config.h` is the single repository-owned beginner configuration point for this model.

The PA5 Arduino D13 line also drives the Nucleo user LED circuitry. This is an intentional reference-board tradeoff because ST documents D13 as SPI1_SCK; users chasing the cleanest signal integrity or custom high-speed layouts should move to a suitable alternate SPI pin set in their own board/config layer after checking the STM32H533 datasheet, alternate-function table, and their exact PCB.

Validation remains **TEMPLATE / integration configuration present**. This mapping has not yet been recorded as STM32CubeH5 BUILD-VERIFIED or NUCLEO-H533RE + ADS1299 BOARD-VERIFIED.
