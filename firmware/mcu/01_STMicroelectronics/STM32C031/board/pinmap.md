# Reference pin map

Board: **Select an official STM32C031 evaluation board**

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

## Beginner mapping convention

For the standardized STM32C031 beginner path, configure the actual pins in CubeMX and give the control GPIOs these exact user labels:

| Logical `SDK_*` signal | CubeMX user label |
|---|---|
| `SDK_CS` | `ADS1299_CS` |
| `SDK_DRDY` | `ADS1299_DRDY` |
| `SDK_RESET` | `ADS1299_RESET` |
| `SDK_PWDN` | `ADS1299_PWDN` |
| `SDK_START` | `ADS1299_START` |

SCK/MOSI/MISO are selected through the chosen SPI peripheral alternate functions. Optional `SDK_TX` is selected through the chosen output transport.

After CubeMX generates the project, `board_config.h` is the single repository file a beginner normally edits. It maps the generated SPI/UART handle names and the labeled GPIO symbols into the STM32C031 ADS1299 integration. The exact physical header pin remains board-specific and must be verified against the selected ST board schematic.
