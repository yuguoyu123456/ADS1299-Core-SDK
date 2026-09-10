# Reference pin map

Board: **Select an official STM32F767 evaluation board**

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

## Concrete starter mapping: NUCLEO-F767ZI

For the standardized beginner path, this model folder now uses **ST NUCLEO-F767ZI (MB1137, STM32F767ZIT6)** as its reference board. The earlier `SDK_*` table above is retained as a generic/legacy template; new reference-board bring-up should use the concrete mapping below and `board_config.h`.

| ADS1299 signal | STM32F767 pin | NUCLEO-144 label | Configuration |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PD14 | D10 | GPIO output, active low |
| DRDY | PD15 | D9 | GPIO input / EXTI, active low |
| RESET | PF12 | D8 | GPIO output, active low |
| PWDN | PF13 | D7 | GPIO output, active low |
| START | PE9 | D6 | GPIO output |
| stream TX | PD8 | ST-LINK VCP | USART3_TX |
| stream RX | PD9 | ST-LINK VCP | USART3_RX |

SPI1 must be configured as **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, software NSS**. The reference board's ST-LINK virtual COM path uses USART3 on PD8/PD9; verify the board solder-bridge routing if VCP output is unavailable.

For another STM32F767 board, change CubeMX routing and `board_config.h` rather than shared ADS1299 core files. Keep one CS per ADS1299 when extending the default shared-SPI architecture to multiple devices.
