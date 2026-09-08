# Reference pin map

Board: **EK-TM4C1294XL**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PA2 | SPI clock | 3.3 V digital |
| MOSI / DIN | PA4 / SSI0XDAT0 TX | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PA5 / SSI0XDAT1 RX | ADS1299 to MCU | 3.3 V digital |
| CS | PA3 (provisional GPIO) | active-low chip select | 3.3 V digital |
| DRDY | PL3 (provisional) | active-low data ready | 3.3 V digital |
| RESET | PL0 (provisional) | active-low reset | 3.3 V digital |
| PWDN | PL1 (provisional) | active-low power-down | 3.3 V digital |
| START | PL2 (provisional) | conversion control | 3.3 V digital |
| stream TX | Debugger snapshot only | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
