# STM32F429 board configuration

## Reference board

The reference path for this model is **STM32F429I-DISC1 (MB1075)** using the
**STM32F429ZIT6**.  ST documents this board as the current order code replacing
the older STM32F429I-DISCO.  Use STM32CubeMX / STM32CubeIDE with STM32CubeF4.

Validation status for this board layer: **TEMPLATE / integration configuration
present**.  The repository configuration is not a claim of ADS1299 hardware
verification.

## The one file a beginner should edit

Start with `board_config.h`.  It is the repository-owned board-dependent
configuration point for SPI, GPIO and host transport choices.  When using a
different STM32F429 board, change CubeMX routing and this board/config layer;
do not edit `firmware/core_driver/ads1299/` merely because pins differ.

## Starter digital wiring

Configure SPI1 as **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first** and use
software-controlled chip select.

| ADS1299 signal | STM32F429 starter GPIO | Direction |
|---|---|---|
| SCLK | PA5 / SPI1_SCK | MCU -> ADS1299 |
| DOUT | PA6 / SPI1_MISO | ADS1299 -> MCU |
| DIN | PA7 / SPI1_MOSI | MCU -> ADS1299 |
| CS | PB0 | MCU -> ADS1299, active low |
| DRDY | PB1 | ADS1299 -> MCU, active low |
| RESET | PB11 | MCU -> ADS1299, active low |
| PWDN | PB12 | MCU -> ADS1299, active low |
| START | PB13 | MCU -> ADS1299 |
| stream TX | PA9 / USART1_TX | MCU -> host |
| stream RX | PA10 / USART1_RX | optional host -> MCU |

Connect digital grounds and ensure the ADS1299 digital I/O supply is compatible
with the MCU GPIO voltage.  This table describes the starter MCU routing, not a
promise that every MB1075 revision exposes every signal without interaction
with onboard circuitry.  Check the exact board schematic and CubeMX pinout
before wiring.

## STM32F429I-DISC1-specific cautions

The Discovery board already uses many STM32F429 pins for its TFT LCD, external
SDRAM, MEMS sensor and ST-LINK/VCP.  Treat pin availability as a board-level
constraint.  If the starter mapping conflicts with an onboard function in the
configuration you need, select another valid STM32F429 alternate function or
GPIO and change only CubeMX plus `board_config.h`.

For host logs/streaming, STM32F429I-DISC1 provides ST-LINK/V2-B virtual COM
routing through **USART1 PA9/PA10** when the corresponding solder-bridge path is
connected.  Do not assume USART2 is the board VCP path.

## CubeMX checklist

1. Select STM32F429ZITx or the matching STM32F429I-DISC1 target configuration.
2. Configure SPI1 master, full duplex, Mode 1, 8-bit, MSB-first, software NSS.
3. Configure CS, RESET, PWDN and START as push-pull outputs with safe startup
   levels; configure DRDY as an input, optionally with EXTI for advanced use.
4. Configure USART1 on PA9/PA10 if using the ST-LINK VCP starter transport.
5. Generate STM32CubeF4 HAL code.
6. Bind the generated handles/GPIO callbacks through `../ads1299_port/` and the
   model examples rather than editing shared ADS1299 core files.

## Performance note

At 250 SPS with one ADS1299, a simple blocking starter transport is acceptable
for bring-up.  For sustained high-rate acquisition or multiple ADS1299 devices,
use DMA/interrupt-driven acquisition and a bounded queue/ring buffer.  Never
block host/radio/file transport inside the DRDY timing path, and count/report
buffer overflow explicitly.

## Expected beginner progression

Once the model examples are wired to this board layer, the intended flow is:
probe/ID -> internal test -> input short -> 250-SPS EEG -> packet stream -> clean
stop.  The board layer must not duplicate ADS1299 register setup; named profiles
and register behavior belong to the shared core.

## Validation status

- Board/config structure: **present**
- STM32Cube software build for this exact starter mapping: **not yet recorded**
- STM32F429I-DISC1 + ADS1299 physical-board verification: **not yet recorded**
- Sustained acquisition / multi-ADS1299 verification: **not yet recorded**
