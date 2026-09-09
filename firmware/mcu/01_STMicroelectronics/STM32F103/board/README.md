# STM32F103 board configuration

## Reference board

Use **NUCLEO-F103RB (MB1136, STM32F103RBT6)** as the first reproducible STM32F103 reference target.

This folder owns only hardware-dependent choices. ADS1299 register values, profiles, frame parsing and device behavior stay in the shared driver.

## One file to edit

For the reference board, the repository-owned configuration point is:

`board/board_config.h`

For another STM32F103 board, normally change only:

1. STM32CubeMX pin/peripheral routing and the five GPIO labels listed below;
2. `ADS1299_BOARD_SPI_HANDLE` if SPI1 is not used;
3. `ADS1299_BOARD_UART_HANDLE` or `ADS1299_BOARD_USE_UART_STREAM` if the transport changes.

Do **not** edit `firmware/core_driver/ads1299/` just to move pins or peripherals.

## Reference wiring

| ADS1299 signal | NUCLEO-F103RB MCU pin | Arduino header | Direction |
|---|---|---|---|
| SCLK | PA5 / SPI1_SCK | D13 | MCU -> ADS1299 |
| DOUT / MISO | PA6 / SPI1_MISO | D12 | ADS1299 -> MCU |
| DIN / MOSI | PA7 / SPI1_MOSI | D11 | MCU -> ADS1299 |
| CS | PB6 | D10 | MCU -> ADS1299 |
| START | PB10 | D6 | MCU -> ADS1299 |
| PWDN | PB4 | D5 | MCU -> ADS1299 |
| RESET | PB5 | D4 | MCU -> ADS1299 |
| DRDY | PA10 | D2 | ADS1299 -> MCU |
| optional stream TX | PA2 / USART2_TX | D1 | MCU -> host |
| digital ground | GND | GND | common reference |

Confirm ADS1299 DVDD / digital-I/O compatibility and the exact analog/power wiring of the ADS1299 board before connecting it. The Nucleo pin map above covers only the digital control/transport path.

## STM32CubeMX / STM32CubeIDE setup

Configure **SPI1**:

- Master
- Full duplex
- 8-bit data size
- MSB first
- clock polarity: Low
- clock phase: 2 Edge
- software NSS / GPIO chip select

That corresponds to ADS1299 SPI Mode 1: CPOL=0, CPHA=1.

Configure GPIO labels exactly as:

- `ADS1299_CS` — output, inactive/high after initialization
- `ADS1299_RESET` — output, inactive/high after initialization
- `ADS1299_PWDN` — output, inactive/high after initialization
- `ADS1299_START` — output
- `ADS1299_DRDY` — input; ADS1299 data-ready is active low

If the beginner streaming example uses UART, enable USART2 TX on PA2 and keep `ADS1299_BOARD_UART_HANDLE` as `huart2`.

Generate the Cube project normally; the generated `main.h` must expose the matching `*_Pin` and `*_GPIO_Port` definitions used by `board_config.h`.

## Expected software path

The intended model integration is:

`board_config.h -> STM32F103 HAL/port layer -> shared ads1299_port_t -> shared ADS1299 core -> examples`

The board layer must not duplicate ADS1299 register sequences.

## Fixed data-size contracts

The reference configuration records these repository contracts for bounded buffers:

- ADS1299-8 continuous raw frame: **27 bytes**
- canonical SDK packet: **49 bytes**
- initial small-MCU stream queue: **4 frames**

The four-frame queue is a conservative starter setting, not a hardware-validation claim.

## Validation status

**TEMPLATE / reference integration configuration**

- Board/config files present: yes
- Official NUCLEO-F103RB reference mapping documented: yes
- Host port self-test infrastructure: present in `../ads1299_port/`
- STM32CubeIDE BUILD-VERIFIED: **no**
- NUCLEO-F103RB + ADS1299 BOARD-VERIFIED: **no**
- sustained acquisition / electrical / EMC / production validation: **not claimed**

Before promoting this model to BUILD-VERIFIED, compile a real generated STM32CubeIDE project containing the shared core and STM32F103 integration. Before promoting it to BOARD-VERIFIED, record evidence from the stated physical board and ADS1299 hardware.

## Sources

Reference the current ST documentation for NUCLEO-F103RB / MB1136 when checking connector routing, board revisions, solder bridges and debugger/VCP behavior. ADS1299 protocol details remain governed by the current TI ADS1299-family datasheet and the shared driver contract.
