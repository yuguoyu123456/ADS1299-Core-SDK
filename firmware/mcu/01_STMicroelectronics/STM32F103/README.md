# STM32F103 ADS1299 Port

Global ecosystem rank: **101**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32F103 / STM32F103
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official STM32F103 evaluation board
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

## 第 101 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 STM32F103 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：兼顾已有工程迁移、低功耗采集及较新高性能系列，具体供货周期待选型时核实。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.st.com/en/microcontrollers-microprocessors/STM32-32-bit-arm-cortex-mcus.html) · [101–200 总清单](../../ECOSYSTEM_101_200.md)

---

## Current roll-up — integration candidate-complete

The original planning notes above are intentionally preserved for project history. The current repository state is more complete than that initial scaffold.

### Reference target

- MCU: **STM32F103RBT6**
- Reference board: **NUCLEO-F103RB (MB1136)**
- Toolchain path: **STM32CubeMX + STM32CubeIDE / STM32CubeF1 HAL**
- One obvious board/config entry point: `board/board_config.h`

### Current beginner path

A new student should follow this order:

1. Open `board/README.md` and wire the documented NUCLEO-F103RB digital interface.
2. Configure SPI1 as Mode 1, 8-bit, MSB-first and create the GPIO labels documented by `board/board_config.h`.
3. Add the shared ADS1299 core and this model's `ads1299_port/` and `examples/` sources to the generated STM32CubeIDE project.
4. Use the STM32F103 platform binding in `examples/stm32f103_example_platform.c`; no shared-core edits are required for normal board bring-up.
5. Run `stm32f103_ads1299_beginner_demo(...)` for the progressive flow: hardware reset -> probe/ID -> internal test -> input short -> 250-SPS EEG -> framed streaming -> clean stop.
6. Use `tests/Makefile.host` and `ads1299_port/Makefile.host` for host-side integration/adapter checks when a host compiler is available.

### Repository-owned pieces now present

- `board/board_config.h` — pins/peripheral handles/transport choice
- `ads1299_port/` — SPI/GPIO/DRDY/delay adapter and host self-test
- `examples/stm32f103_example_platform.c/.h` — STM32F1 HAL binding
- `examples/stm32f103_beginner_demo.c/.h` — beginner bring-up/acquisition flow
- `tests/` — frame/register/SPI/reset/DRDY integration smoke-test infrastructure
- shared ADS1299 behavior remains in `../../../core_driver/ads1299/`

### Validation status

**TEMPLATE / integration candidate-complete**

- repository file/integration structure: present
- beginner path avoids shared-core edits: yes
- host test infrastructure: present
- STM32CubeIDE BUILD-VERIFIED: **no**
- NUCLEO-F103RB + ADS1299 BOARD-VERIFIED: **no**
- sustained acquisition / multi-ADS1299 / electrical / EMC / production validation: **not claimed**

Do not interpret candidate-complete as hardware support evidence. The next promotion step is a clean generated STM32CubeIDE build using the stated reference configuration; physical-board evidence is required separately for BOARD-VERIFIED status.
