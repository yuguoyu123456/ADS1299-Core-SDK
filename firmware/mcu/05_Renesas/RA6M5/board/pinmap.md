# Reference pin map

Board: **EK-RA6M5**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | P412 / PMOD2 SCK signal; header unverified | SPI clock | 3.3 V digital |
| MOSI / DIN | P411 / PMOD2 MOSI signal; header unverified | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | P410 / PMOD2 MISO signal; header unverified | ADS1299 to MCU | 3.3 V digital |
| CS | SDK_CS | active-low chip select | 3.3 V digital |
| DRDY | SDK_DRDY | active-low data ready | 3.3 V digital |
| RESET | SDK_RESET | active-low reset | 3.3 V digital |
| PWDN | SDK_PWDN | active-low power-down | 3.3 V digital |
| START | SDK_START | conversion control | 3.3 V digital |
| stream TX | SDK_TX | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
