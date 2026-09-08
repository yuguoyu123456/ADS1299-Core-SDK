# AT32F403A ADS1299 Port

Global ecosystem rank: **184**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: Artery
- Family / MCU: AT32F403A / AT32F403A
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official AT32F403A evaluation board
- Official environment: AT32 device firmware library
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

## 第 184 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 AT32F403A 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：补充国内 AT32 产品梯度；按具体器件核对 SPI/DMA 与开发板。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.arterychip.com/) · [101–200 总清单](../../ECOSYSTEM_101_200.md)
