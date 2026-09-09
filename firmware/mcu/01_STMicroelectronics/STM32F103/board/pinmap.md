# Reference pin map

Board: **Select an official STM32F103 evaluation board**

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

## Current reference implementation: NUCLEO-F103RB

The generic `SDK_*` table above is retained for compatibility with the original
planning scaffold. The current reproducible STM32F103 beginner reference is
**NUCLEO-F103RB (MB1136, STM32F103RBT6)**.

| ADS1299 signal | STM32F103RBT6 pin | NUCLEO-F103RB Arduino pin | Peripheral / role |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PB6 | D10 | GPIO output, active low |
| START | PB10 | D6 | GPIO output |
| PWDN | PB4 | D5 | GPIO output, active low |
| RESET | PB5 | D4 | GPIO output, active low |
| DRDY | PA10 | D2 | GPIO input, active low |
| stream TX | PA2 | D1 | USART2_TX, optional |
| ground | GND | GND | common digital reference |

Configure SPI1 as master/full-duplex, 8-bit, MSB-first, CPOL Low and CPHA
2Edge (SPI Mode 1). Use software-controlled CS.

In STM32CubeMX assign the GPIO user labels `ADS1299_CS`, `ADS1299_RESET`,
`ADS1299_PWDN`, `ADS1299_START` and `ADS1299_DRDY`. `board_config.h` consumes
the generated `*_Pin` and `*_GPIO_Port` names so a board change does not require
editing the shared ADS1299 driver.

ST's current MB1136 user manual lists PA5/D13 as SPI1_SCK, PA6/D12 as
SPI1_MISO, PA7/D11 as SPI1_MOSI, PB6/D10 as SPI1_CS-capable GPIO, PB10/D6,
PB4/D5, PB5/D4, PA10/D2 and PA2/D1/USART2_TX on the NUCLEO-F103RB Arduino
headers. Check the exact board revision and solder-bridge configuration before
hardware use.

Validation: **TEMPLATE / reference integration configuration**. The mapping is
documented from the official board manual, but this repository configuration
has not yet been promoted to STM32CubeIDE BUILD-VERIFIED or physical
NUCLEO-F103RB + ADS1299 BOARD-VERIFIED status.
