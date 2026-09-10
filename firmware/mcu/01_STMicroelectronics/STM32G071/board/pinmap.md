# Reference pin map

Board: **NUCLEO-G071RB (MB1360, STM32G071RBT6)**

This concrete starter mapping replaces the need for a beginner to invent the former `SDK_*` placeholders. The official MB1360 Arduino connector mapping documents SPI1 on PA5/PA6/PA7 and D10 on PB0; UART2 PA2/PA3 is routed to the ST-LINK Virtual COM port in the default configuration.

| Signal | Reference pin | Header / role | Function | Level |
|---|---|---|---|---|
| SCK | PA5 | Arduino D13 | SPI1_SCK | 3.3 V digital |
| MOSI / DIN | PA7 | Arduino D11 | SPI1_MOSI | 3.3 V digital |
| MISO / DOUT | PA6 | Arduino D12 | SPI1_MISO | 3.3 V digital |
| CS | PB0 | Arduino D10 | software GPIO chip select | 3.3 V digital |
| DRDY | PC7 | Arduino D9 | active-low input / EXTI candidate | 3.3 V digital |
| RESET | PA9 | Arduino D8 | active-low GPIO output | 3.3 V digital |
| PWDN | PA8 | Arduino D7 | active-low GPIO output | 3.3 V digital |
| START | PB14 | Arduino D6 | conversion-control GPIO output | 3.3 V digital |
| stream TX | PA2 | ST-LINK VCP | UART2_TX | 3.3 V digital |
| stream RX | PA3 | ST-LINK VCP | UART2_RX | 3.3 V digital |
| GND | GND | common ground | digital reference | 0 V |

## Required peripheral settings

- SPI1: master, full-duplex, 8-bit, **Mode 1 (CPOL=0, CPHA=1)**, MSB-first.
- CS remains software-controlled. This preserves the repository's default multi-device architecture: shared SPI bus with an independent CS per ADS1299.
- DRDY is active-low. Polling is acceptable for first bring-up; sustained acquisition should use a short ISR/EXTI path plus a bounded queue.
- UART2 starter streaming: 460800 baud. A 49-byte canonical packet at 250 SPS requires 12,250 payload bytes/s; 115200 8N1 is insufficient for an unthrottled stream.

## Board-specific cautions

PA5 also drives the NUCLEO-G071RB USER LED circuitry. This does not change the ADS1299 SPI contract, but users designing a custom STM32G071 carrier may prefer another datasheet-valid SPI routing after validating their board.

The ADS1299 digital I/O supply and MCU GPIO voltage must be compatible. Confirm header routing, solder bridges, debugger/VCP routing and the exact board schematic before connecting hardware. The earlier `SDK_*` convention remains useful for custom vendor projects, but the reference-board path above is now the recommended beginner starting point.

For other STM32G071 boards, modify the CubeMX routing and `board_config.h`; do not modify shared ADS1299 register/model/frame files merely to change pins.
