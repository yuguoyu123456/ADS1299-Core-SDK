# Reference pin map

Board: **ESP32-DevKitC V4 / 4 MB WROOM reference**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GPIO18 / J3-9 | SPI clock | 3.3 V digital |
| MOSI / DIN | GPIO23 / J3-2 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GPIO19 / J3-8 | ADS1299 to MCU | 3.3 V digital |
| CS | GPIO21 / J3-6 | active-low chip select | 3.3 V digital |
| DRDY | GPIO27 / J2-11 | active-low data ready | 3.3 V digital |
| RESET | GPIO22 / J3-3 | active-low reset | 3.3 V digital |
| PWDN | GPIO25 / J2-9 | active-low power-down | 3.3 V digital |
| START | GPIO26 / J2-10 | conversion control | 3.3 V digital |
| stream TX | UART0; no EEG stream implemented | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
