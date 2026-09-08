# Reference pin map

Board: **NUCLEO-F091RC (MB1136, STM32F091RCT6)**

This is the reference wiring used by the STM32F091 beginner integration. Board-
specific handles and generated GPIO labels are centralized in
`board_config.h`; changing boards must not require ADS1299 core edits.

| Signal | STM32 pin / peripheral | Nucleo header | Function | Level |
|---|---|---|---|---|
| SCK | PA5 / SPI1_SCK | D13 | SPI clock | 3.3 V digital |
| MOSI / DIN | PA7 / SPI1_MOSI | D11 | MCU to ADS1299 | 3.3 V digital |
| MISO / DOUT | PA6 / SPI1_MISO | D12 | ADS1299 to MCU | 3.3 V digital |
| CS | PB6 / GPIO | D10 | active-low chip select | 3.3 V digital |
| START | PB10 / GPIO | D6 | conversion control | 3.3 V digital |
| PWDN | PB4 / GPIO | D5 | active-low power-down | 3.3 V digital |
| RESET | PB5 / GPIO | D4 | active-low reset | 3.3 V digital |
| DRDY | PA10 / GPIO input | D2 | active-low data ready | 3.3 V digital |
| stream TX | PA2 / USART2_TX | D1 | optional host transport | 3.3 V digital |
| GND | GND | GND | common digital reference | 0 V |

In STM32CubeMX, label the GPIOs `ADS1299_CS`, `ADS1299_START`,
`ADS1299_PWDN`, `ADS1299_RESET`, and `ADS1299_DRDY`. Configure SPI1 as master,
full duplex, 8-bit, MSB-first, CPOL Low and CPHA 2 Edge (SPI Mode 1). Use
software chip-select control.

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm
the actual ADS1299 carrier power rails, connector orientation, Nucleo solder
bridges and any peripheral conflicts before applying power. This document does
not define the ADS1299 analog supply, reference, BIAS or electrode wiring.

The previous generic `SDK_*` placeholders are superseded by this documented
reference configuration. For a custom STM32F091 board, keep the same logical
signals and update only the CubeMX project plus `board_config.h` as needed.
