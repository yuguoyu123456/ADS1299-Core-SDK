# STM32F407 board configuration

## Reference board

The reference path for this model is **STM32F407G-DISC1 (MB997, STM32F407VGT6)** using STM32CubeMX / STM32CubeIDE.

Validation status: **TEMPLATE / integration configuration present**. This repository does not yet record a successful STM32CubeIDE build or physical ADS1299 run on this board.

## One file to edit

For a different STM32F407 board, keep the shared ADS1299 core unchanged. Change the generated CubeMX pin/peripheral routing and, when handles differ, edit only `board_config.h`.

The reference project expects these CubeMX GPIO labels:

- `ADS1299_CS`
- `ADS1299_RESET`
- `ADS1299_PWDN`
- `ADS1299_START`
- `ADS1299_DRDY`

`board_config.h` consumes the generated `*_Pin` and `*_GPIO_Port` definitions and selects the SPI/UART handles.

## Reference wiring

| ADS1299 signal | STM32F407VG signal | Direction | Notes |
|---|---|---|---|
| SCLK | PA5 / SPI1_SCK | MCU -> ADS1299 | SPI Mode 1 |
| DOUT | PA6 / SPI1_MISO | ADS1299 -> MCU | MSB-first |
| DIN | PA7 / SPI1_MOSI | MCU -> ADS1299 | MSB-first |
| CS | PB0 | MCU -> ADS1299 | active low |
| DRDY | PB1 | ADS1299 -> MCU | active low |
| RESET | PB11 | MCU -> ADS1299 | active low |
| PWDN | PB12 | MCU -> ADS1299 | active low |
| START | PB13 | MCU -> ADS1299 | conversion control |
| stream TX | PA2 / USART2_TX | MCU -> host | optional transport |

Use compatible digital supply levels and a common digital ground. Analog ADS1299 power/reference/electrode design is outside this board GPIO mapping.

### Discovery-board conflict check

On STM32F407G-DISC1, some MCU pins are also connected to on-board peripherals. In particular PA5/PA6/PA7 appear in the MB997 on-board I/O circuitry as well as the expansion headers. Before wiring an ADS1299, check the schematic for the exact MB997 revision and ensure the selected pins are not being actively driven or undesirably loaded. If necessary, choose another valid SPI/pin mapping in CubeMX and change only the board configuration/port integration.

## CubeMX configuration

Configure the selected SPI peripheral as:

- Master
- Full duplex
- 8-bit data
- MSB first
- clock polarity Low
- clock phase 2 Edge
- software-controlled NSS/CS

This is ADS1299 **SPI Mode 1 (CPOL=0, CPHA=1)**.

Configure CS, RESET, PWDN and START as GPIO outputs. Configure DRDY as a GPIO input or EXTI source as required by the application. The initial beginner path can poll DRDY; higher-rate/multi-device code should use a bounded ISR/DMA buffering strategy rather than blocking host transport in the timing path.

## Data contracts

The board configuration preserves the repository contracts:

- ADS1299-8 raw continuous frame: **27 bytes**
- canonical repository packet: **49 bytes**
- starter queue: **16 bounded frames**

Shared register/profile behavior belongs to `firmware/core_driver/ads1299/`; a new student should not edit those files for board bring-up.

## Next layer

After CubeMX generation and board configuration, use `../ads1299_port/` for the STM32F407 platform callbacks and `../examples/` for the model-level examples as they are completed.
