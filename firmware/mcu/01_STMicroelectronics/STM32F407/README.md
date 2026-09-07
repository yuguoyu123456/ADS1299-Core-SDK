# STM32F407VGT6 ADS1299 Port

Global ecosystem rank: **2**. Status: **Compiles**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32F4 / STM32F407VGT6
- Architecture: Arm Cortex-M4F
- Reference board: STM32F407G-DISC1 (MB997)
- Official environment: STM32CubeF4
- Compiler: GNU Arm Embedded via PlatformIO / STM32CubeIDE

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


- STM32Cube binding: `stm32cube_adapter/`
- Reproducible compile record: `build.md`
- Official references and license provenance: `sources.md`

The checked board is the official MB997 STM32F407G-DISC1. SPI1 is shared with
the on-board LIS3DSH; keep its PE3 chip select high when communicating with the
external ADS1299.
