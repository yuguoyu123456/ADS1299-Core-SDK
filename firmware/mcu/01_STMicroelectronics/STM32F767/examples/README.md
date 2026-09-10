# STM32F767 ADS1299 examples

Reference path: **NUCLEO-F767ZI (MB1137, STM32F767ZIT6)** using STM32CubeMX / STM32CubeIDE and STM32CubeF7 HAL.

## Recommended beginner entry point

Use `stm32f767_ads1299_beginner_demo()` from `stm32f767_beginner_demo.c` after CubeMX-generated board/peripheral initialization.

Suggested first call:

```c
#include "stm32f767_beginner_demo.h"

int rc = stm32f767_ads1299_beginner_demo(1000u);
```

`1000u` streams 1000 canonical packets and then performs a clean STOP/SDATAC. Use `0u` only when you intentionally want an endless stream.

The demo progresses through:

1. portable STM32F767 port initialization;
2. ADS1299 hardware reset and SDATAC;
3. ADS1299-family ID probe (4/6/8 channel variants);
4. internal test source, 8 frames;
5. input-short validation, 8 frames;
6. 250-SPS, gain-24 normal-input configuration;
7. DRDY-paced continuous reads;
8. canonical 49-byte packet encoding and USART3 VCP transport;
9. clean STOP followed by SDATAC.

The shared ADS1299 core owns commands, register semantics, models, frame parsing and packet encoding. Do not copy or edit shared register logic in this folder.

## Files to add to the Cube project

Model-local files:

- `../ads1299_port/ads1299_spi.c`
- `../ads1299_port/ads1299_gpio.c`
- `../ads1299_port/ads1299_drdy.c`
- `stm32f767_example_platform.c`
- `stm32f767_beginner_demo.c`

Shared ADS1299 files required by the APIs used here must also be part of the project, including the core driver, model/frame support and canonical packet implementation already maintained under `firmware/core_driver/ads1299/`.

Keep the original `main_ads1299.c` if desired; `stm32f767_example_platform.c` supplies its legacy `board_ads1299_hal()` callback as a compatibility path.

## CubeMX configuration for the reference board

Use `../board/board_config.h` and `../board/README.md` as the hardware source of truth.

For the starter path configure:

- SPI1: master, 8-bit, MSB-first, **CPOL=0 / CPHA=1 (SPI Mode 1)**;
- PA5 = SPI1_SCK;
- PA6 = SPI1_MISO;
- PA7 = SPI1_MOSI;
- PD14 = software CS, output, idle high;
- PD15 = DRDY, input;
- PF12 = RESET, output;
- PF13 = PWDN, output;
- PE9 = START, output;
- USART3 on the board's ST-LINK VCP route;
- USART3 = 460800 baud, 8 data bits, no parity, 1 stop bit.

The 460800-baud starter setting is deliberate. A 49-byte packet at 250 SPS is 12,250 payload bytes/s; 115200 baud with 8N1 can carry only about 11,520 payload bytes/s and therefore cannot sustain the stream.

## What success looks like

Before binary packet streaming begins, the log should progress through messages equivalent to:

```text
ADS1299 STM32F767 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4 and ADS1299-6 are accepted and reported by their corresponding ID messages.

## Diagnostics

- `ERR RESET`: check ADS1299 power, PWDN and RESET wiring.
- `ERR SDATAC` or `ERR ID`: check SPI Mode 1, software CS, clock, power and MISO/MOSI/SCK wiring.
- `ERR DRDY timeout`: check DRDY routing and whether conversion was started.
- `ERR frame read`: check SPI/CS integrity while RDATAC is active.
- `ERR stream transport`: verify USART3, ST-LINK VCP routing and the configured baud rate.

## F767 sustained-acquisition note

This starter uses blocking HAL SPI/UART so the first board bring-up is easy to inspect. It is not the final architecture for high-rate or multi-device acquisition. For sustained acquisition, move the DRDY timing path to IRQ/DMA and write into a bounded queue/ring buffer with explicit overflow counters. Drain UART/USB/Ethernet outside that timing-critical path. If DMA touches cacheable SRAM on STM32F7, apply the appropriate cache coherency strategy for the selected memory region and DMA direction.

For multiple ADS1299 devices, preserve the repository default architecture of shared SPI with independent software-controlled CS signals unless a different topology is intentionally documented and verified.

## Validation status

**TEMPLATE / repository integration present.**

The source path is now complete enough for Cube integration, but this README does **not** claim a recorded STM32CubeIDE clean build, physical NUCLEO-F767ZI + ADS1299 execution, sustained-stream validation, DMA/cache validation, or multi-ADS1299/64-channel board validation.
