# Reference pin map

Board: **MIMXRT1060-EVKB**

| Signal | Reference pin | Function | Level |
|---|---|---|---|
| SCK | GPIO_SD_B0_00 / J17-6 | SPI clock | 3.3 V digital |
| MOSI / DIN | GPIO_SD_B0_02 / J17-4 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | GPIO_SD_B0_03 / J17-5 | ADS1299 to MCU | 3.3 V digital |
| CS | GPIO_SD_B0_01 / J17-3 | active-low chip select | 3.3 V digital |
| DRDY | GPIO_AD_B0_11 / J16-3 | active-low data ready | 3.3 V digital |
| RESET | GPIO_AD_B1_08 / J16-4 | active-low reset | 3.3 V digital |
| PWDN | GPIO_AD_B0_09 / J16-5 | active-low power-down | 3.3 V digital |
| START | GPIO_AD_B0_10 / J16-6 | conversion control | 3.3 V digital |
| stream TX | OpenSDA VCOM (transport only) | optional host transport | 3.3 V digital |

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
header routing, boot straps, solder bridges and debugger conflicts against the
exact board schematic before connecting hardware. `SDK_*` names mean the user
must select and document an available pin in the consuming vendor project.
