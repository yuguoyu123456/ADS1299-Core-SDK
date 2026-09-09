# STM32F429 ADS1299 Port

Global ecosystem rank: **105**. Legacy catalog status: **Planned**. Current repository roll-up: **TEMPLATE / integration candidate-complete**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32F429 / **STM32F429ZIT6** reference path
- Architecture: Arm Cortex-M4 with FPU, up to 180 MHz
- Reference board: **STM32F429I-DISC1 (MB1075)**
- Official environment: STM32CubeMX / STM32CubeIDE + STM32CubeF4
- Compiler: STM32CubeIDE bundled GNU Arm toolchain or another ST-supported compiler

The STM32F429ZI family provides up to 2 MB Flash, up to 256+4 KB SRAM including 64 KB CCM, a 16-stream DMA controller, and up to six SPI peripherals. These device capabilities are planning headroom, not ADS1299 throughput evidence.

## Quick Start

1. Start from **STM32F429I-DISC1 / STM32F429ZIT6** in STM32CubeMX or STM32CubeIDE.
2. Open `board/board_config.h`. This is the one obvious repository-owned file to change for board pins, SPI instance and starter transport.
3. Configure SPI1 as **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, software NSS** and reproduce the GPIO routing documented in `board/README.md` / `board/pinmap.md`.
4. Add the shared ADS1299 core from `../../../core_driver/ads1299/`, this model's `ads1299_port/`, and the progressive example sources from `examples/` to the generated STM32 project.
5. Initialize Cube HAL peripherals, then call `stm32f429_ads1299_beginner_demo(1000u)`; use `0u` for continuous streaming.
6. Expected progression is **probe/ID -> internal test -> input short -> 250-SPS EEG -> canonical packet stream -> clean stop**.
7. If using another STM32F429 board, change CubeMX routing and `board/board_config.h`; do not edit shared ADS1299 register/model/frame files merely because pins differ.

## ADS1299 connection

The starter routing is PA5/PA6/PA7 for SPI1 SCK/MISO/MOSI, PB0 CS, PB1 DRDY, PB11 RESET, PB12 PWDN and PB13 START. The reference starter host stream uses USART1 PA9/PA10. Confirm the exact MB1075 revision, solder bridges and onboard-function conflicts before wiring.

Use software-controlled CS. Keep DRDY, RESET, PWDN and START as independent GPIOs. Start with conservative SPI timing until ID read, configuration/readback and internal-test behavior are correct.

## Repository layers

- ADS1299 behavior: `../../../core_driver/ads1299/`
- One board/config edit point: `board/board_config.h`
- STM32F429 hardware adapter: `ads1299_port/`
- Progressive beginner flow: `examples/stm32f429_beginner_demo.c`
- HAL binding: `examples/stm32f429_example_platform.c`
- Legacy minimal call flow retained for compatibility: `examples/main_ads1299.c`
- Host/integration smoke tests: `tests/`
- Vendor-project procedure: `integration.md`

The platform layer never owns ADS1299 register policy. UART, USB, BLE or Ethernet transport must stay outside the shared ADS1299 core and must not block the DRDY timing path. For sustained high-rate or multi-device acquisition, use DMA/interrupt-driven acquisition plus bounded buffering and explicit overflow accounting.

## Validation status

- Board/config layer: **present**
- STM32F429 HAL/portable-port binding: **present**
- Progressive probe/test/EEG/stream example: **present**
- Host integration test sources/build recipe: **present**
- Host test PASS recorded: **no**
- Clean STM32CubeIDE build for this progressive reference path: **not yet recorded**
- STM32F429I-DISC1 + ADS1299 physical-board verification: **not yet recorded**
- Sustained acquisition / multi-ADS1299 verification: **not yet recorded**

The original rank 105 / Planned entry is retained as catalog history; it is not the current technical-completeness assessment. The number is a maintenance ordering identifier, not a global sales rank.

[ST STM32F429ZI product page](https://www.st.com/en/microcontrollers-microprocessors/stm32f429zi.html) · [STM32F429 Discovery kit](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) · [101–200 catalog](../../ECOSYSTEM_101_200.md)
