# Reference pin map

Board: **ESP32-C6-DevKitC-1**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GPIO6 / J1-5 | SPI clock | 3.3 V digital |
| MOSI / DIN | GPIO7 / J1-6 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GPIO2 / J1-12 | ADS1299 to MCU | 3.3 V digital |
| CS | GPIO10 / J1-10 | active-low chip select | 3.3 V digital |
| DRDY | GPIO3 / J1-13 | active-low data ready | 3.3 V digital |
| RESET | GPIO18 / J3-10 | active-low reset | 3.3 V digital |
| PWDN | GPIO19 / J3-9 | active-low power-down | 3.3 V digital |
| START | GPIO1 / J1-8 | conversion control | 3.3 V digital |
| stream TX | UART0 TX; no stream implemented | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
