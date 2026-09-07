# Reference pin map

Board: **ESP32-S3-DevKitC-1-N8**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GPIO12 / J1-18 | SPI clock | 3.3 V digital |
| MOSI / DIN | GPIO11 / J1-17 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GPIO13 / J1-19 | ADS1299 to MCU | 3.3 V digital |
| CS | GPIO10 / J1-16 | active-low chip select | 3.3 V digital |
| DRDY | GPIO9 / J1-15 | active-low data ready | 3.3 V digital |
| RESET | GPIO8 / J1-12 | active-low reset | 3.3 V digital |
| PWDN | GPIO6 / J1-6 | active-low power-down | 3.3 V digital |
| START | GPIO7 / J1-7 | conversion control | 3.3 V digital |
| stream TX | firmware/transport backend | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
