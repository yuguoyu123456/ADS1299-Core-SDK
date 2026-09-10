# Reference pin map

Board: **NUCLEO-G474RE; provisional control GPIO profile**

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

## Concrete repository reference mapping

The generic `SDK_*` table above is retained for compatibility with earlier notes. For the current standardized beginner path, use `board_config.h` as the authoritative repository-owned configuration point and the concrete NUCLEO-G474RE mapping below.

| ADS1299 signal | STM32G474RE pin | Nucleo label | Role |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PB6 | D10 | GPIO, active low |
| DRDY | PC7 | D9 | input/EXTI, active low |
| RESET | PA9 | D8 | GPIO, active low |
| PWDN | PA8 | D7 | GPIO, active low |
| START | PB10 | D6 | GPIO conversion control |
| stream TX | PA2 | STLINK-V3E VCP | LPUART1_TX in documented default bridge configuration |
| stream RX | PA3 | STLINK-V3E VCP | LPUART1_RX in documented default bridge configuration |

ST UM2505 documents the Arduino header mapping for NUCLEO-G474RE and the selectable VCP solder-bridge routing. The repository reference path uses SPI Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first and software-controlled CS.

For sustained 250-SPS canonical streaming, the starter transport is 460800 baud. The 49-byte canonical packet requires 12,250 payload bytes/s at 250 SPS; 115200 baud/8N1 cannot carry that stream losslessly.

The older `reference_image.*` path is retained as a separate cold-reset/reference experiment and uses a different provisional pin set. Do not mix its PB3/PB4/PB5/PC0..PC4 wiring with the standardized beginner mapping above. New integrations should start from `board_config.h`.
