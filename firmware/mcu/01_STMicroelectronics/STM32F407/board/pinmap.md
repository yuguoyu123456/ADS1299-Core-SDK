# Reference pin map

Board: **STM32F407G-DISC1 (MB997)**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PA5 / P1-15 | SPI clock | 3.3 V digital |
| MOSI / DIN | PA7 / P1-17 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PA6 / P1-18 | ADS1299 to MCU | 3.3 V digital |
| CS | PB0 / P1-22 | active-low chip select | 3.3 V digital |
| DRDY | PB1 / P1-21 | active-low data ready | 3.3 V digital |
| RESET | PB11 / P1-35 | active-low reset | 3.3 V digital |
| PWDN | PB12 / P1-36 | active-low power-down | 3.3 V digital |
| START | PB13 / P1-37 | conversion control | 3.3 V digital |
| stream TX | PA2 / P1-14 (transport only) | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.

## Current reference configuration

`board_config.h` is now the single repository-owned board configuration point for the STM32F407 beginner path. The reference MCU mapping is PA5/PA6/PA7 for SPI1, PB0 for CS, PB1 for DRDY, PB11 for RESET, PB12 for PWDN, PB13 for START, and optional PA2/USART2_TX for host transport.

Configure SPI as Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, with software-controlled CS. The ADS1299-8 continuous raw frame contract is 27 bytes; the canonical repository packet is 49 bytes.

### MB997 on-board peripheral caution

PA5, PA6 and PA7 are also connected into on-board Discovery-kit peripheral circuitry on MB997 revisions. Treat the table above as the software reference mapping, not a guarantee that every physical board revision is conflict-free without checking its schematic/solder-bridge population. Before connecting ADS1299 hardware, verify the exact MB997 revision. If another SPI/pin mapping is more appropriate, change CubeMX plus `board_config.h`; do not modify the shared ADS1299 core.

Validation remains **TEMPLATE / integration configuration present** until a documented STM32Cube build succeeds. Physical hardware behavior is **not BOARD-VERIFIED**.
