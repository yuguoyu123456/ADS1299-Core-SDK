# Reference pin map

Board: **LP-MSPM0G3507**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PB9 / SPI1-SCK | SPI clock | 3.3 V digital |
| MOSI / DIN | PB8 / SPI1-PICO | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PB7 / SPI1-POCI | ADS1299 to MCU | 3.3 V digital |
| CS | PB6 / SPI1-CS0 remuxed GPIO | active-low chip select | 3.3 V digital |
| DRDY | PA25 / GPIO input | active-low data ready | 3.3 V digital |
| RESET | PA26 / GPIO output | active-low reset | 3.3 V digital |
| PWDN | PA27 / GPIO output | active-low power-down | 3.3 V digital |
| START | PA28 / GPIO output | conversion control | 3.3 V digital |
| stream TX | PA10 / XDS110 VCOM TX (transport only) | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
