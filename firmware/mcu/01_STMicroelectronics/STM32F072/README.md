# STM32F072 ADS1299 Port

Global ecosystem rank: **102**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32F072 / STM32F072
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official STM32F072 evaluation board
- Official environment: STM32Cube / official device package
- Compiler: vendor-supported compiler

## ADS1299 connection

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep CS software-controlled and
route DRDY, RESET, PWDN and START as independent GPIOs. Start at 4 MHz or less
until ID read, configuration readback and the internal test signal pass. The
reference pin assignment is documented in `board/pinmap.md`; confirm it against
the exact board revision before wiring.

## Repository layers

- ADS1299 behavior: `../../../core_driver/ads1299/`
- This platform's hardware-only adapter: `ads1299_port/`
- Minimal call flow: `examples/main_ads1299.c`
- Vendor-project procedure: `integration.md`

The port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also
provides a millisecond helper without changing the stable Core port contract.
It never defines ADS1299 registers. UART, USB, BLE or Ethernet transport stays
in `firmware/transport/` and must not block a DRDY handler.

## 第 102 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 STM32F072 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：兼顾已有工程迁移、低功耗采集及较新高性能系列，具体供货周期待选型时核实。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.st.com/en/microcontrollers-microprocessors/STM32-32-bit-arm-cortex-mcus.html) · [101–200 总清单](../../ECOSYSTEM_101_200.md)

---

## CURRENT ROLL-UP STATUS (supersedes the legacy planning notes above)

The STM32F072 model folder now contains a concrete reference integration and is no longer only a callback scaffold. The legacy planning text above is retained for repository history; this section describes the current path.

**Reference board:** NUCLEO-F072RB (MB1136, STM32F072RBT6)  
**MCU:** Arm Cortex-M0, up to 48 MHz, 128 KiB Flash, 16 KiB SRAM  
**Vendor environment:** STM32CubeF0 / STM32CubeMX / STM32CubeIDE  
**Current validation level:** **TEMPLATE / integration candidate-complete**. Host adapter/unit-test infrastructure exists, but a full STM32CubeIDE target build and physical ADS1299 board run are not claimed yet.

### 1. What to wire

Use the reference table in `board/README.md`. The default reproducible mapping is SPI1 on PA5/PA6/PA7, with software CS plus independent START, PWDN, RESET and DRDY GPIOs. Configure SPI as 8-bit, MSB-first, CPOL Low, CPHA 2Edge (ADS1299 SPI Mode 1).

### 2. What a beginner edits

For the reference board, configure CubeMX with the GPIO labels documented by `board/board_config.h`. For another STM32F072 board, change only CubeMX pin/peripheral assignments and, when needed, the handle aliases in `board/board_config.h`.

Do **not** edit `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared ADS1299 behavior for a normal board change.

### 3. Files to add to the CubeIDE project

- shared ADS1299 core: `../../../core_driver/ads1299/`
- canonical packet encoder: `../../../common/data_packet/ads1299_packet.c/.h`
- STM32F072 port: `ads1299_port/*.c` and headers
- board configuration: `board/board_config.h`
- beginner example: `examples/stm32f072_example_platform.c/.h` and `examples/stm32f072_beginner_demo.c/.h`

Vendor-generated startup code, linker scripts, CMSIS/HAL and `.ioc` ownership remain in the STM32CubeIDE project rather than being invented in this repository.

### 4. Beginner bring-up sequence

After `HAL_Init()`, clock setup, GPIO, SPI1 and the selected output transport are initialized, call:

```c
#include "stm32f072_beginner_demo.h"

(void)stm32f072_ads1299_beginner_demo(1000u);
```

The demo performs the intended progression:

`reset -> SDATAC -> probe/ID -> internal test -> input short -> EEG 250 SPS -> continuous frame acquisition -> canonical 49-byte packet stream -> clean stop`

A successful communicating ADS1299-family part should first produce readable stage logs such as `OK ID ADS1299-8`, `OK internal-test frames`, `OK input-short frames` and `OK EEG 250 SPS configured`. After the stream stage begins, the selected UART path carries binary repository packets rather than printable EEG text.

### 5. Tests

`ads1299_port/` contains a host adapter self-test. `tests/` contains a shared-core + STM32F072-port smoke-test runner and Makefile. These are software validation paths only; they do not imply STM32CubeIDE target build success or electrical validation.

### 6. First failure checks

- ID/SDATAC failure: SPI Mode 1, MSB-first, CS, ADS1299 clock, power and digital I/O voltage.
- DRDY timeout: START/conversion state, DRDY wiring and active-low interpretation.
- RESET failure: RESET/PWDN polarity and delays.
- Frame failure: CS timing, full-duplex SPI transfer and expected ADS1299-family frame length.
- Stream failure: output peripheral initialization, baud rate and transport backpressure.

The current model-level objective is now a real CubeIDE target build followed by NUCLEO-F072RB + ADS1299 hardware evidence; until those are captured this folder must not be labeled BUILD-VERIFIED or BOARD-VERIFIED.
