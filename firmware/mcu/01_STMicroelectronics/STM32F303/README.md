# STM32F303 ADS1299 Port

Global ecosystem rank: **104**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32F303 / STM32F303
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official STM32F303 evaluation board
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

## 第 104 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 STM32F303 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：兼顾已有工程迁移、低功耗采集及较新高性能系列，具体供货周期待选型时核实。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.st.com/en/microcontrollers-microprocessors/STM32-32-bit-arm-cortex-mcus.html) · [101–200 总清单](../../ECOSYSTEM_101_200.md)

---

## Current roll-up (supersedes the initial scaffold status above)

The initial planning notes above are intentionally preserved as project history. The current repository state is now an **integration candidate**, not merely an empty scaffold.

### Reference path

- Reference board: **NUCLEO-F303RE (MB1136)**
- MCU: **STM32F303RET6**, Arm Cortex-M4F, up to 72 MHz
- Reference device memory class: **512 KB Flash, 80 KB total SRAM including 16 KB CCM**
- Vendor flow: **STM32CubeMX + STM32CubeIDE / STM32CubeF3 HAL**
- Board/config entry: `board/board_config.h`
- Hardware adapter: `ads1299_port/`
- Beginner platform binding: `examples/stm32f303_example_platform.c`
- Beginner flow: `examples/stm32f303_beginner_demo.c`
- Host integration tests: `tests/Makefile.host`
- Port self-test: `ads1299_port/Makefile.host`

### Beginner path

1. Start an STM32CubeMX/STM32CubeIDE project for NUCLEO-F303RE.
2. Apply the SPI1/GPIO/USART2 setup documented in `board/README.md`.
3. Treat `board/board_config.h` as the single repository-owned hardware configuration point.
4. Add the shared ADS1299 core, this model's `ads1299_port` sources, and the beginner example sources.
5. Run `stm32f303_ads1299_beginner_demo(...)` to exercise probe/ID, internal-test, input-short, 250-SPS EEG acquisition, canonical packet streaming and clean stop.
6. For another STM32F303 board, change the CubeMX peripheral/pin routing and board config/handles only; normal bring-up must not require editing `ads1299.c`, `ads1299_regs.h` or `ads1299_model.c`.

### Validation status

- **TEMPLATE / repository integration present:** yes.
- **Host-test recipes present:** yes; presence is not a recorded pass.
- **STM32CubeIDE BUILD-VERIFIED:** no claim yet.
- **NUCLEO-F303RE + ADS1299 BOARD-VERIFIED:** no claim yet.
- **Sustained acquisition / overflow characterization:** no claim yet.
- **Multi-ADS1299 / 64-channel validation:** no claim yet.

The model is therefore **integration candidate-complete for this repository-filling round**, but it is not yet build-verified or board-verified.