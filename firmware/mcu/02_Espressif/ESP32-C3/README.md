# ESP32-C3 ADS1299 Port

Global ecosystem rank: **121**. Status: **Planned**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: Espressif
- Family / MCU: ESP32-C3 / ESP32-C3
- Architecture: Confirm exact CPU/core variant in official device documentation
- Reference board: Select an official ESP32-C3 evaluation board
- Official environment: ESP-IDF
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

## 第 121 项：后续开发入口

当前是 **Planned** 目录和通用回调模板，尚未实现 ESP32-C3 的官方 SDK 绑定。
编号是项目维护顺序，不是全球销量排名，也不表示未来供货保证。

选型理由：补充联网采集和本地处理选型；各型号无线能力不同，不能默认全部带 Wi-Fi。

先确定完整料号、封装、板卡和官方 SDK，再补充真实 SPI/GPIO/DRDY 适配。
CPU/RAM/Flash/SPI 上限、DMA、USB/BLE 与多 ADS1299 能力均以具体器件为准。
通用 examples/main_ads1299.c 需要板级 board_ads1299_hal，当前不能独立链接运行。
tests/ 是待运行的 Core/接口测试入口，不是该 MCU 编译或硬件测试记录。

[官方资料入口](https://www.espressif.com/en/products/socs) · [101–200 总清单](../../ECOSYSTEM_101_200.md)

## Current reference implementation (supersedes the earlier Planned note)

The folder now contains a concrete ESP-IDF reference path for the official
**ESP32-C3-DevKitM-1**. The earlier Planned text above is retained as historical
context, but is no longer the current integration state.

Edit only `board/esp32c3_devkitm1_ads1299.h` when adapting pins to another
ESP32-C3 board. The default reference wiring is:

| ADS1299 | ESP32-C3-DevKitM-1 |
|---|---|
| DOUT / MISO | GPIO3 |
| DIN / MOSI | GPIO7 |
| SCLK | GPIO6 |
| CS | GPIO10 |
| DRDY | GPIO4 |
| RESET | GPIO5 |
| PWDN | GPIO0 |
| START | GPIO1 |

The defaults avoid GPIO2/GPIO8/GPIO9 strapping-sensitive assignments, GPIO8's
on-board RGB LED, USB Serial/JTAG GPIO18/GPIO19, and UART0 GPIO20/GPIO21.

### Build / flash / run

```bash
cd firmware/mcu/02_Espressif/ESP32-C3/examples/esp_idf_reference
idf.py set-target esp32c3
idf.py build
idf.py -p <serial-port> flash monitor
```

The reference application uses the shared ADS1299 core and executes this
progressive path without requiring edits to core register files:

1. hardware reset + SDATAC;
2. ID/family probe;
3. internal-test capture;
4. input-short capture;
5. typed 250-SPS, gain-24, normal-input channel configuration;
6. RDATAC/START continuous EEG acquisition;
7. a fixed 16-frame queue between the acquisition and lower-priority transport tasks.

Expected successful monitor output includes `probe OK`, diagnostic frame lines,
and `beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream`.
Queue overflow is explicit through dropped/high-watermark diagnostics rather
than silently overwriting unread EEG frames.

### Validation status

- **TEMPLATE / IMPLEMENTED:** ESP-IDF SPI/GPIO HAL, reference-board config,
  progressive example, bounded acquisition queue and shared-core build wiring are present.
- **BUILD-VERIFIED:** not yet established for the documented ESP-IDF toolchain/reference configuration.
- **BOARD-VERIFIED:** not established; no physical ESP32-C3-DevKitM-1 + ADS1299 run is claimed.
