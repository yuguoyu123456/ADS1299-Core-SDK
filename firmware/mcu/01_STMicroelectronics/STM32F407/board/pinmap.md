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
