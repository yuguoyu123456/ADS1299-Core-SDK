# STM32C031 board configuration

This directory is the **only board-specific edit point** for the STM32C031 beginner path. Do not change ADS1299 register logic in `firmware/core_driver/ads1299/` when moving to another STM32C031 board.

## 1. Configure CubeMX / CubeIDE

Create or open the official STM32C031 vendor project and configure one SPI master for ADS1299:

- 8-bit data size
- MSB first
- CPOL = Low
- CPHA = 2 Edge
- software-controlled chip select (NSS is not used as the ADS1299 CS policy)

The CPOL/CPHA combination above is ADS1299 **SPI Mode 1**.

Give the GPIO signals these exact CubeMX user labels so the generated `main.h` symbols match `board_config.h`:

| ADS1299 signal | CubeMX user label | Direction | Active level |
|---|---|---|---|
| CS | `ADS1299_CS` | output | low |
| RESET | `ADS1299_RESET` | output | low |
| PWDN | `ADS1299_PWDN` | output | low |
| START | `ADS1299_START` | output | high to start conversion |
| DRDY | `ADS1299_DRDY` | input / optional EXTI | low |

SCK, MOSI/DIN and MISO/DOUT are assigned by the selected SPI peripheral and its alternate-function pins.

## 2. Edit one file

Open `board_config.h` and normally change only:

```c
#define ADS1299_BOARD_SPI_HANDLE      hspi1
#define ADS1299_BOARD_USE_UART_STREAM 1
#define ADS1299_BOARD_UART_HANDLE     huart1
```

If your CubeMX-generated handles use other names, replace those aliases. The GPIO aliases do not need manual changes when the exact CubeMX labels above are used.

If UART streaming is not required, set `ADS1299_BOARD_USE_UART_STREAM` to `0` and let the application select another repository transport.

## 3. Wiring checklist

Connect MCU SCK -> ADS1299 SCLK, MCU MOSI -> ADS1299 DIN, MCU MISO <- ADS1299 DOUT, plus CS, DRDY, RESET, PWDN and START according to the labeled GPIOs. Confirm the exact evaluation-board header pins from that board's ST schematic before wiring; this SDK deliberately does not invent a universal STM32C031 header assignment.

The MCU digital I/O level must be compatible with ADS1299 DVDD. Verify common ground, ADS1299 clock source, analog/digital supplies and reference circuitry before interpreting communication failures as software defects.

## 4. What the board layer does not contain

This folder must not duplicate ADS1299 register values, EEG profiles, ID parsing or frame decoding. Those behaviors belong to the shared ADS1299 core. The STM32C031 `ads1299_port` directory converts the generated STM32 HAL operations into the repository's portable `ads1299_port_t` interface.

## 5. First failure checks

If probe/ID fails, check in this order:

1. SPI is CPOL=Low, CPHA=2Edge, MSB first.
2. CS is software-controlled and goes low for the complete ADS1299 transaction.
3. RESET and PWDN are not being held low.
4. DRDY is wired to the configured input and is treated as active-low.
5. SCK/MOSI/MISO alternate-function pins match the selected SPI instance.
6. ADS1299 DVDD and MCU GPIO levels are compatible and ground is common.

## Validation status

`board_config.h` is a repository-owned integration template built around CubeMX-generated labels and handle aliases. It does **not** claim validation on every STM32C031 evaluation board, because pin routing varies by exact board/package. Real-board wiring must be checked against the selected ST board schematic.
