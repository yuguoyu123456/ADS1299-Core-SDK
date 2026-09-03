# Platform support matrix

ADS1299-Core-SDK uses one common ADS1299 device driver and complete per-board MCU/FPGA reference projects. A platform is **not** called supported because a folder or a small port file exists.

## Current complete-project track

| Platform family | Repository target | Build status | Hardware status | Notes |
|---|---|---|---|---|
| ST STM32 | `firmware/mcu/st/stm32f407_black/` | **Compiles** | Pre-hardware-validation | STM32Cube HAL / PlatformIO reference |
| Espressif ESP32 | `firmware/mcu/espressif/esp32s3_devkitc/` | **Compiles** | Pre-hardware-validation | Native ESP-IDF reference |
| Raspberry Pi MCU | `firmware/mcu/raspberry_pi/rp2040_pico/` | **Compiles** | Pre-hardware-validation | Pico SDK reference |
| WCH | `firmware/mcu/wch/ch32v307_evt/` | **Compiles** | Pre-hardware-validation | CH32V30x / WCH NoneOS SDK / PlatformIO |

All four projects above have passed clean GitHub Actions builds from fresh runner environments. They do **not** become `Bench-tested` until real ADS1299 hardware is connected and the documented bring-up checklist passes.

## Global MCU roadmap

| Platform family | Representative targets | Planned ecosystem |
|---|---|---|
| STMicroelectronics | STM32F1 / F4 / G4 / H7 | HAL first; LL/bare-metal where useful |
| GigaDevice | GD32F4 / GD32E5 | vendor SDK reference project |
| WCH | CH32V203 / CH32V307 | RISC-V priority; CH32V307 reference now in complete-project track |
| Espressif | ESP32 / ESP32-S3 | ESP-IDF |
| Nordic Semiconductor | nRF52840 / nRF5340 | nrfx / nRF Connect SDK |
| Raspberry Pi | RP2040 / RP2350 | Pico SDK |
| NXP | MCX / i.MX RT | MCUXpresso |
| Microchip 32-bit | SAM / PIC32 | MPLAB ecosystem |
| Renesas | RA / RX | FSP / e2 studio |
| Texas Instruments MCU | MSPM0 / TM4C / CC13xx-CC26xx | representative SDK projects |
| Infineon / Cypress | XMC / PSoC | ModusToolbox where appropriate |
| Silicon Labs | EFM32 / EFR32 | Gecko SDK |
| HDSC | HC32 | representative vendor SDK project |
| Artery | AT32 | representative vendor SDK project |
| MindMotion | MM32 | representative vendor SDK project |
| Nationstech | N32 | representative vendor SDK project |
| Puya | PY32 | low-cost MCU track |
| Geehy | APM32 | representative vendor SDK project |
| STC | STC8 / STC32 | educational / lower-rate project where appropriate |
| Microchip AVR | ATmega / AVR Dx | educational / lower-rate project where appropriate |

## FPGA track

- AMD/Xilinx — complete board project + RTL + constraints + simulation target
- Intel/Altera — complete board project + RTL + constraints + simulation target
- Lattice — complete board project + RTL + constraints + simulation target

## Status vocabulary

- **Planned** — roadmap only.
- **Compiles** — clean build verified with the documented toolchain.
- **Bench-tested** — real ADS1299 hardware verified.
- **24h-tested** — long-duration acquisition verified.

## Minimum acceptance for a visible MCU project

1. Complete build system and executable entry point.
2. Exact board and pin map documented.
3. SPI Mode 1 configured correctly.
4. ADS1299 reset and device-ID read.
5. Register read/write verified.
6. Internal test signal acquired on all 8 channels.
7. Full 27-byte frame decoded correctly.
8. Signed 24-bit channel conversion verified.
9. Common packet output with sequence/integrity checking.
10. PC decoder/recorder compatibility.
11. Real hardware evidence before `Bench-tested`.
12. Long-duration evidence before `24h-tested`.
