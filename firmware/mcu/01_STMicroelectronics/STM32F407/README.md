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
- Minimal legacy call flow: `examples/main_ads1299.c`
- Beginner end-to-end flow: `examples/stm32f407_beginner_demo.c`
- Single board/config entry point: `board/board_config.h`
- Vendor-project procedure: `integration.md`
- Host/integration tests: `tests/`

The port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also
provides a millisecond helper without changing the stable Core port contract.
It never defines ADS1299 registers. UART, USB, BLE or Ethernet transport stays
outside the ADS1299 Core and must not block a DRDY handler.

- STM32Cube binding: `stm32cube_adapter/`
- Reproducible compile record: `build.md`
- Official references and license provenance: `sources.md`

The checked board is the official MB997 STM32F407G-DISC1. SPI1 is shared with
the on-board LIS3DSH; keep its PE3 chip select high when communicating with the
external ADS1299.

## Beginner Quick Start

1. Use an STM32F407G-DISC1 (MB997) or a board with the same MCU.
2. Configure the board from `board/board_config.h`; this is the normal place to
   change SPI/GPIO/UART routing for another STM32F407 board.
3. Add the shared ADS1299 Core, `ads1299_port/`, and the example platform/demo
   sources listed in `examples/README.md` to the Cube project.
4. Initialize Cube-generated GPIO/SPI/UART peripherals.
5. Call `stm32f407_ads1299_beginner_demo(1000u)`.
6. Confirm ID/probe, internal-test, input-short, 250-SPS EEG configuration and
   canonical streaming diagnostics in that order.
7. Use `0u` instead of `1000u` for continuous streaming.

A new user should not edit `ads1299.c`, register definitions, model logic or
shared frame/packet code for normal board bring-up.

## Current validation

- **BUILD-VERIFIED:** yes, for the documented PlatformIO/STM32CubeF4 reference
  build recorded in `build.md` on 2026-09-04.
- **BOARD-VERIFIED with ADS1299:** no evidence recorded yet.
- **Beginner demo CubeIDE build record:** not yet recorded separately.
- **Sustained acquisition / multi-ADS1299 hardware validation:** not yet recorded.

The current folder is an integration candidate for the beginner acceptance gate:
board/config, port, runnable progressive example, diagnostics and host/integration
test infrastructure are present. Hardware status must not be inferred from the
successful software build.
