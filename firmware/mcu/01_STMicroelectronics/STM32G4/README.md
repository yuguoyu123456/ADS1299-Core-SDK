# STM32G474RE ADS1299 Port

Global ecosystem rank: **11**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32G4 / STM32G474RE
- Architecture: Arm Cortex-M4F
- Reference board: NUCLEO-G474RE; provisional control GPIO profile
- Official environment: STM32CubeG4
- Compiler: GCC Arm Embedded 9.2.1; CubeIDE/Keil not run

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

## Maintained G474 reference

Use cmsis_adapter/ with STM32G474RE and the original cold-reset HSI16 board/reference_image.c. See build.md, sources.md and board/reference_image.md. Bounded hardware SPI, 1 MHz, no HAL/IRQ/DMA sharing. Software link and modeled faults passed; board headers and electrical operation are unverified.

---

## Standardized beginner path (current maintenance path)

The recommended reference board for the standardized SDK path is **NUCLEO-G474RE (MB1367 / STM32G474RET6)**.

For a new student, start here:

1. Open `board/README.md` and wire the ADS1299 using the standardized mapping.
2. Treat `board/board_config.h` as the one obvious repository-owned board/config file. A custom STM32G4 board should normally require CubeMX peripheral/pin changes plus corresponding board/config values, not shared ADS1299 core edits.
3. Create the official STM32CubeG4 project and configure SPI1 Mode 1, software CS, the control GPIOs and LPUART1/VCP as described in `examples/README.md`.
4. Add `ads1299_port/`, the beginner example sources, the shared ADS1299 core and `firmware/common/data_packet/ads1299_packet.c`.
5. Call `stm32g4_ads1299_beginner_demo(1000u)` after Cube HAL/peripheral initialization. Passing `0u` requests continuous streaming.
6. Expect probe/ID, internal-test, input-short and 250-SPS/gain-24 configuration diagnostics before canonical binary streaming.

The progressive path is:

`NUCLEO-G474RE -> board/board_config.h -> STM32G4 HAL/portable port -> shared ADS1299 core -> ID/probe -> internal-test -> input-short -> 250-SPS EEG -> canonical packet stream -> STOP/SDATAC`

The beginner path must not require edits to `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared register/driver internals.

### Standardized reference mapping

- SPI1: PA5 SCK, PA6 MISO, PA7 MOSI
- CS: PB6, idle high
- DRDY: PC7, active low
- RESET: PA9
- PWDN: PA8
- START: PB10
- Host transport: LPUART1 PA2/PA3 through the board VCP configuration
- Starter stream: 460800 baud, 8-N-1

The original `cmsis_adapter/` and `board/reference_image.*` cold-reset experiment remains preserved as a separate advanced/reference path. Do not mix its provisional pin profile with the standardized beginner wiring.

## Current maintenance status

**Integration candidate-complete; not yet target BUILD-VERIFIED or BOARD-VERIFIED.**

Present in the model folder:

- explicit reference board and board/config layer;
- portable ADS1299 port implementation;
- progressive probe/internal-test/input-short/250-SPS/stream example;
- shared canonical packet integration;
- host/integration smoke-test recipes;
- legacy debugger and CMSIS reference paths preserved.

Not yet claimed:

- a recorded successful STM32CubeIDE target build for the standardized beginner project;
- physical NUCLEO-G474RE + ADS1299 execution;
- sustained DMA/EXTI acquisition evidence;
- multi-ADS1299 or 64-channel board verification.
