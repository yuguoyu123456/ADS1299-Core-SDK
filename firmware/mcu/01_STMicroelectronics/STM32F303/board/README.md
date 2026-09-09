# STM32F303 board configuration

## Reference board

Use **NUCLEO-F303RE (MB1136, STM32F303RET6)** as the reference board for this model folder.

This directory is the board-specific layer. A beginner should normally change only STM32CubeMX pin/peripheral routing and `board_config.h`; do not edit the shared ADS1299 core to move to another STM32F303 board.

## Reference wiring

| ADS1299 signal | NUCLEO-F303RE MCU pin | Arduino header | Direction |
|---|---:|---:|---|
| SCLK | PA5 | D13 | MCU -> ADS1299 |
| DOUT / MISO | PA6 | D12 | ADS1299 -> MCU |
| DIN / MOSI | PA7 | D11 | MCU -> ADS1299 |
| CS | PB6 | D10 | MCU -> ADS1299 |
| START | PB10 | D6 | MCU -> ADS1299 |
| PWDN | PB4 | D5 | MCU -> ADS1299 |
| RESET | PB5 | D4 | MCU -> ADS1299 |
| DRDY | PA10 | D2 | ADS1299 -> MCU |
| optional stream TX | PA2 / USART2_TX | D1 | MCU -> host |

Use a common digital ground and ensure the ADS1299 digital I/O supply is compatible with the STM32 3.3 V domain. This table covers the digital control/data interface only; the ADS1299 analog supply, reference, BIAS/electrode protection and patient-safety design are outside this board-pin file.

## STM32CubeMX / STM32CubeIDE setup

1. Start a project for **NUCLEO-F303RE**.
2. Configure **SPI1** as master, full duplex, 8-bit, MSB-first, clock polarity Low and clock phase 2Edge. That is SPI Mode 1 (CPOL=0, CPHA=1).
3. Configure PB6, PB10, PB4 and PB5 as push-pull GPIO outputs and give them the labels `ADS1299_CS`, `ADS1299_START`, `ADS1299_PWDN` and `ADS1299_RESET`.
4. Configure PA10 as GPIO input or EXTI input and label it `ADS1299_DRDY`.
5. If using the starter UART stream path, enable USART2 and keep `ADS1299_BOARD_UART_HANDLE` as `huart2`. The standard Nucleo board also uses USART2 for its ST-LINK virtual COM connection, so verify the exact solder-bridge/header routing if you need the physical Arduino D1 pin rather than the virtual COM path.
6. Generate code. Include this model folder's ADS1299 port/example sources plus the repository shared ADS1299 core described by the model-level integration documentation.
7. Keep all board changes in CubeMX and `board_config.h`. Do not modify shared ADS1299 register/model files for board bring-up.

## What to edit for another STM32F303 board

Normally only:

- GPIO/peripheral assignment in the vendor project;
- `ADS1299_BOARD_SPI_HANDLE` if SPI1 is not used;
- `ADS1299_BOARD_UART_HANDLE` or `ADS1299_BOARD_USE_UART_STREAM` for transport;
- generated GPIO labels/pins corresponding to CS, RESET, PWDN, START and DRDY.

The ADS1299 command/register/profile behavior remains in the shared core.

## Fixed interface assumptions

- SPI Mode 1, MSB-first.
- DRDY active-low.
- CS, RESET and PWDN active-low.
- ADS1299-8 raw continuous frame: 27 bytes.
- Canonical repository packet where used by examples: 49 bytes.
- Starter streaming queue: 8 bounded static frames.

## Validation status

- **TEMPLATE / integration configuration:** present.
- **BUILD-VERIFIED:** no claim yet for STM32CubeIDE/reference configuration.
- **BOARD-VERIFIED:** no claim yet for NUCLEO-F303RE + physical ADS1299 hardware.

Do not interpret the reference wiring as electrical-safety, EMC, long-run or production validation.
