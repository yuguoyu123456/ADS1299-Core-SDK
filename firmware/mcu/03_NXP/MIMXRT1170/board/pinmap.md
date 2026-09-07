# Reference pin map

Board: **MIMXRT1170-EVKB**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GPIO_AD_28 / J10-12 | SPI clock | 3.3 V digital |
| MOSI / DIN | GPIO_AD_30 / J10-8 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GPIO_AD_31 / J10-10 | ADS1299 to MCU | 3.3 V digital |
| CS | Arduino D10 / J10-6 | active-low chip select | 3.3 V digital |
| DRDY | Arduino D2 / physical pin pending verification | active-low data ready | 3.3 V digital |
| RESET | Arduino D3 / physical pin pending verification | active-low reset | 3.3 V digital |
| PWDN | Arduino D4 / physical pin pending verification | active-low power-down | 3.3 V digital |
| START | Arduino D5 / physical pin pending verification | conversion control | 3.3 V digital |
| stream TX | MCU-Link VCOM (transport only) | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
