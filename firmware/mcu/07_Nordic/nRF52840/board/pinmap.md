# Reference pin map

Board: **nRF52840 DK**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | P1.03 | SPI clock | 3.3 V digital |
| MOSI / DIN | P1.05 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | P1.04 | ADS1299 to MCU | 3.3 V digital |
| CS | P1.06 | active-low chip select | 3.3 V digital |
| DRDY | P1.07 | active-low data ready | 3.3 V digital |
| RESET | P1.08 | active-low reset | 3.3 V digital |
| PWDN | P1.10 | active-low power-down | 3.3 V digital |
| START | P1.11 | conversion control | 3.3 V digital |
| stream TX | P1.12 | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
