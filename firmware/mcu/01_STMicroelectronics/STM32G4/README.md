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
