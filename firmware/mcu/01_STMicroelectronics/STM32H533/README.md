# STM32H533 ADS1299 Port

Global ecosystem rank: **111**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32H533 / STM32H533
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official STM32H533 evaluation board
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

## 第 111 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 STM32H533 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：兼顾已有工程迁移、低功耗采集及较新高性能系列，具体供货周期待选型时核实。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.st.com/en/microcontrollers-microprocessors/STM32-32-bit-arm-cortex-mcus.html) · [101–200 总清单](../../ECOSYSTEM_101_200.md)

---

## Standardized SDK maintenance status

The historical `Planned` text above is retained for catalog compatibility. The current maintenance state is **integration-candidate-complete**: this model now has a reference-board configuration, portable MCU port, progressive beginner example, and host integration-test recipe. This does **not** mean target build verification or physical-board verification.

### Reference beginner path

Reference board: **NUCLEO-H533RE (STM32H533RET6)**.

1. Start in `board/README.md` and use `board/board_config.h` as the single repository-owned board/configuration point.
2. Configure the consuming STM32CubeH5 project for the documented SPI1/GPIO/USART2 mapping. SPI must remain Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first with software-controlled CS.
3. Add the model-local sources from `ads1299_port/` and `examples/`, plus the shared ADS1299 core and canonical packet source documented in `integration.md`.
4. Call `stm32h533_ads1299_beginner_demo(1000u)` for a finite first run. Passing `0u` selects continuous streaming.
5. Expected progression is: reset/SDATAC -> ID/probe -> internal test -> input short -> 250-SPS gain-24 EEG -> canonical packet stream -> STOP/SDATAC.
6. If using another STM32H533 board, change only the board/configuration and vendor-HAL binding values; do not edit shared ADS1299 register/model/core files for normal bring-up.

### What is already present

- `board/board_config.h`: obvious hardware-dependent configuration point.
- `ads1299_port/`: SPI/GPIO/DRDY/delay adapter that produces the shared `ads1299_port_t` contract.
- `examples/stm32h533_beginner_demo.c`: progressive bring-up and acquisition flow using shared ADS1299 APIs.
- `examples/stm32h533_example_platform.c`: STM32CubeH5 binding plus the legacy `board_ads1299_hal()` compatibility entry.
- `tests/Makefile.host`: host-side integration build recipe covering the platform adapter with shared ADS1299 modules.
- Existing `examples/main_ads1299.c` and earlier generic documentation remain available for compatibility.

### Validation boundary

Current state is **integration-candidate-complete**, not `BUILD-VERIFIED` and not `BOARD-VERIFIED`. No claim is made for sustained EXTI/DMA acquisition, cache-coherency correctness on a chosen DMA memory region, long-run packet-loss behavior, multi-ADS1299 hardware, or 64-channel operation. Record those states only after the corresponding build or physical evidence exists.
