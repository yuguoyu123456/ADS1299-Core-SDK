# STM32F072 board configuration

This folder is the board-specific layer for the STM32F072 ADS1299 port. The recommended reference board for the first reproducible path is **NUCLEO-F072RB (MB1136, STM32F072RBT6)**.

## What a beginner edits

For the documented reference board, configure the pins/peripherals in STM32CubeMX and keep the generated labels shown below. If you use a different STM32F072 board, change only the CubeMX assignments and `board_config.h` handle aliases when necessary. Do not edit the shared ADS1299 driver just to move pins or change SPI/UART instances.

The single repository-owned configuration file is:

`board/board_config.h`

Expected CubeMX GPIO labels:

- `ADS1299_CS`
- `ADS1299_RESET`
- `ADS1299_PWDN`
- `ADS1299_START`
- `ADS1299_DRDY`

## Reference NUCLEO-F072RB wiring

| ADS1299 signal | STM32 pin | Nucleo Arduino pin | Configuration |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT | PA6 | D12 | SPI1_MISO |
| DIN | PA7 | D11 | SPI1_MOSI |
| CS | PB6 | D10 | GPIO output, idle high |
| START | PB10 | D6 | GPIO output |
| PWDN | PB4 | D5 | GPIO output, active low |
| RESET | PB5 | D4 | GPIO output, active low |
| DRDY | PA10 | D2 | GPIO input, active low |
| optional stream TX | PA2 | D1 | USART2_TX / ST-LINK VCP path |

Also connect compatible digital ground and ensure the ADS1299 digital I/O supply is compatible with the MCU GPIO voltage. The table is a repository reference configuration, not evidence of electrical or board-level validation.

## SPI configuration

Use SPI1 master, full duplex, 8-bit data, **MSB first**, **CPOL Low**, **CPHA 2Edge**. That is ADS1299 SPI Mode 1. Chip select is managed as GPIO by the ADS1299 port layer rather than by an automatically pulsed hardware NSS path.

## Why this layer exists

The STM32F072 port code consumes generic callbacks and the shared `ads1299_port_t` contract. Board pin choices must stay here or in the CubeMX-generated board project. ADS1299 register sequences, sampling profiles, frame decoding and multi-device behavior belong to the shared core.

## Small-MCU buffering policy

`board_config.h` records a 27-byte ADS1299-8 raw frame, the repository 49-byte packet size, and a small bounded queue target. This is an integration policy only; application code should still detect and report overflow rather than silently dropping timing-critical samples.

## Validation status

**TEMPLATE / reference integration configuration.** The pin mapping is based on the official NUCLEO-F072RB header mapping, but this repository configuration has not been claimed as STM32CubeIDE build-verified or physical-board verified in this maintenance step.
