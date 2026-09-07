# Reference pin map

Board: **NUCLEO-H743ZI2 (MB1364)**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | PA5 / CN7 D13 | SPI clock | 3.3 V digital |
| MOSI / DIN | PA7 / CN7 D11 (SB33 ON, SB35 OFF) | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PA6 / CN7 D12 | ADS1299 to MCU | 3.3 V digital |
| CS | PD14 / CN7 D10 | active-low chip select | 3.3 V digital |
| DRDY | PF3 / CN7 D8 | active-low data ready | 3.3 V digital |
| RESET | PG12 / CN10 D7 | active-low reset | 3.3 V digital |
| PWDN | PE9 / CN10 D6 | active-low power-down | 3.3 V digital |
| START | PE11 / CN10 D5 | conversion control | 3.3 V digital |
| stream TX | PD8 / ST-LINK VCP | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
