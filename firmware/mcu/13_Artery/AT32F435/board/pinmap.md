# Reference pin map

Board: **AT-START-F435**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PA5 | SPI clock | 3.3 V digital |
| MOSI / DIN | PA7 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PA6 | ADS1299 to MCU | 3.3 V digital |
| CS | PB0 | active-low chip select | 3.3 V digital |
| DRDY | PB1 | active-low data ready | 3.3 V digital |
| RESET | PB2 | active-low reset | 3.3 V digital |
| PWDN | PB10 | active-low power-down | 3.3 V digital |
| START | PB11 | conversion control | 3.3 V digital |
| stream TX | PA9 | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
