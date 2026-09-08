# STM32F072 ADS1299 examples

Validation status: **TEMPLATE / repository integration present**. The files in this folder have not yet been claimed as STM32CubeIDE build-verified or hardware board-verified.

## Recommended first path

Use the `NUCLEO-F072RB` reference configuration documented in `../board/README.md` and `../board/board_config.h`.

The beginner demo deliberately keeps ADS1299 register behavior in the shared core. Board-specific values belong in `../board/board_config.h` and CubeMX-generated GPIO/peripheral configuration.

## Files

- `stm32f072_example_platform.c/.h`: STM32Cube HAL binding for SPI, GPIO, DRDY, delays, UART logging/streaming and timestamping.
- `stm32f072_beginner_demo.c/.h`: complete beginner sequence: probe ID -> internal test -> input-short -> 250-SPS EEG -> canonical packet stream.
- `main_ads1299.c`: older minimal example retained for compatibility. Its `board_ads1299_hal()` dependency is now provided by `stm32f072_example_platform.c`.

## CubeMX / CubeIDE integration

Create an official STM32CubeIDE project for NUCLEO-F072RB and configure:

1. SPI1 master, full duplex, 8-bit, MSB first.
2. Clock polarity Low and clock phase 2Edge: ADS1299 SPI Mode 1.
3. GPIO labels exactly as documented by `../board/board_config.h`: `ADS1299_CS`, `ADS1299_RESET`, `ADS1299_PWDN`, `ADS1299_START`, `ADS1299_DRDY`.
4. USART2 when `ADS1299_BOARD_USE_UART_STREAM` is enabled.
5. Add the repository-owned shared ADS1299 core sources, `firmware/common/data_packet/ads1299_packet.c`, this model's `ads1299_port` sources, and the example sources to the generated project.
6. Add include paths for the shared core, data packet module, `../ads1299_port`, `../board`, and this directory.

After the generated calls to `HAL_Init()`, system clock setup, GPIO, SPI1 and USART2 initialization, call:

```c
#include "stm32f072_beginner_demo.h"

/* Stream 1000 EEG packets and return. Use 0 for continuous streaming. */
(void)stm32f072_ads1299_beginner_demo(1000u);
```

## Expected text sequence

Before binary streaming starts, a correctly communicating ADS1299-family device should produce text similar to:

```text
ADS1299 STM32F072 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical 49-byte packets
```

ADS1299-4 and ADS1299-6 report their corresponding ID line.

After `RUN stream...`, the selected UART carries canonical repository binary packets, not printable EEG text. Each ADS1299-8 packet is 49 bytes and contains sync, version/flags, sequence, timestamp, 3 status bytes, 8 int32 channels and CRC16.

At 250 SPS, 49-byte packets require 12,250 payload bytes/s before UART framing. For the simple blocking UART demonstration, use at least 230400 baud. A production application should replace the blocking transport with interrupt/DMA plus bounded buffering so transport work cannot delay DRDY servicing.

## Failure guide

- `ERR RESET`: check PWDN/RESET polarity, ADS1299 power and wiring.
- `ERR SDATAC` or `ERR ID`: first verify SPI Mode 1, MSB-first, CS and ADS1299 clock/power.
- `ERR DRDY timeout`: verify DRDY wiring and that the device has entered conversion.
- `ERR frame read`: check SPI/CS integrity and frame timing.
- `ERR stream transport`: verify USART2 initialization, baud rate and `board_config.h` transport selection.

Changing to another STM32F072 board should normally require only CubeMX peripheral/pin changes and the board/config binding. Do not edit `ads1299.c`, `ads1299_regs.h` or the shared model logic for a normal board change.
