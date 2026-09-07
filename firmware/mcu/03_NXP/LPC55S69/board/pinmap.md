# Reference pin map

Board: **LPCXpresso55S69**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PIO0_21 / P17-14 | SPI clock | 3.3 V digital |
| MOSI / DIN | PIO0_20 / P17-10 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PIO0_19 / P17-12 | ADS1299 to MCU | 3.3 V digital |
| CS | PIO1_31 (provisional) | active-low chip select | 3.3 V digital |
| DRDY | PIO1_7 (provisional) | active-low data ready | 3.3 V digital |
| RESET | PIO0_26 (provisional) | active-low reset | 3.3 V digital |
| PWDN | PIO0_27 (provisional) | active-low power-down | 3.3 V digital |
| START | PIO1_6 (provisional) | conversion control | 3.3 V digital |
| stream TX | Debugger snapshot only | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
