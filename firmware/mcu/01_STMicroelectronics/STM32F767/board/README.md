# STM32F767 ADS1299 board configuration

## Reference board

Use **ST NUCLEO-F767ZI (MB1137, STM32F767ZIT6)** as the reference bring-up board for this model folder.

The beginner path should edit only `board/board_config.h` plus the STM32CubeMX peripheral routing required to realize the same selections. Shared ADS1299 register/profile/frame code remains under `firmware/core_driver/ads1299/` and should not be edited for normal board bring-up.

## Reference wiring

| ADS1299 signal | NUCLEO-F767ZI pin | Header label | STM32 function |
|---|---|---|---|
| SCLK | PA5 | D13 | SPI1_SCK |
| DOUT / MISO | PA6 | D12 | SPI1_MISO |
| DIN / MOSI | PA7 | D11 | SPI1_MOSI |
| CS | PD14 | D10 | GPIO output, active low |
| DRDY | PD15 | D9 | GPIO input / EXTI, active low |
| RESET | PF12 | D8 | GPIO output, active low |
| PWDN | PF13 | D7 | GPIO output, active low |
| START | PE9 | D6 | GPIO output |
| host TX | PD8 | ST-LINK VCP | USART3_TX |
| host RX | PD9 | ST-LINK VCP | USART3_RX |
| DGND | GND | GND | common digital ground |

Confirm ADS1299 DVDD and MCU digital I/O levels are electrically compatible before connection. This file documents digital routing only; it is not a substitute for the ADS1299 analog/power design requirements.

## STM32CubeMX starter settings

Configure `SPI1` as full-duplex master with software NSS, 8-bit data, MSB first, clock polarity low and clock phase on the second edge (**SPI Mode 1: CPOL=0, CPHA=1**). Choose a conservative SPI baud rate for first bring-up and raise it only after a clean probe/read path is established.

Configure PD14, PF12, PF13 and PE9 as push-pull outputs. Keep CS deasserted high when idle. Configure PD15 as input or falling-edge EXTI for DRDY; ADS1299 DRDY is active low. Configure `USART3` on PD8/PD9 for the ST-LINK virtual COM port, initially at 115200 baud.

The NUCLEO-144 manual documents that USART3 PD8/PD9 can be routed between ST-LINK VCP and the ST Morpho connector using solder bridges. Verify the actual MB1137 revision and solder-bridge state when serial output is missing.

## What to edit for another STM32F767 board

Keep the model hierarchy and shared core unchanged. Normally change only:

1. STM32CubeMX pin/peripheral routing;
2. `board/board_config.h` pin/peripheral selections;
3. the platform/HAL binding only if the generated peripheral handle names differ.

Do not duplicate ADS1299 register setup in this folder.

## Timing and buffering rule

The blocking HAL path is for first 250-SPS bring-up. Sustained acquisition, higher rates and multi-ADS1299 systems should use DRDY interrupt/DMA driven acquisition into a bounded queue/ring buffer. Host UART/USB/network work must not block the DRDY timing path, and queue overflow should be counted and reported explicitly.

## Validation status

**TEMPLATE / integration configuration present.** The reference routing and configuration contract are now concrete, but this folder does not yet claim STM32Cube build verification or physical NUCLEO-F767ZI + ADS1299 board verification.
