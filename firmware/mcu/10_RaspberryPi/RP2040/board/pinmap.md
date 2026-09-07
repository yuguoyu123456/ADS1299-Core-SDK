# Reference pin map

Board: **Raspberry Pi Pico**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GP18 | SPI clock | 3.3 V digital |
| MOSI / DIN | GP19 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GP16 | ADS1299 to MCU | 3.3 V digital |
| CS | GP17 | active-low chip select | 3.3 V digital |
| DRDY | GP20 | active-low data ready | 3.3 V digital |
| RESET | GP21 | active-low reset | 3.3 V digital |
| PWDN | Not wired in native example | active-low power-down | 3.3 V digital |
| START | GP22 | conversion control | 3.3 V digital |
| stream TX | GP0 / UART0 TX | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
