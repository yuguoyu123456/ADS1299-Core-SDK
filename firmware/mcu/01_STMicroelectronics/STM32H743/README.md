# STM32H743ZIT6 ADS1299 Port

Global ecosystem rank: **1**. Status: **Compiles**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32H7 / STM32H743ZIT6
- Architecture: Arm Cortex-M7F
- Reference board: NUCLEO-H743ZI2 (MB1364)
- Official environment: STM32CubeH7
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

The checked reference is specifically the MB1364 NUCLEO-H743ZI2, not the older
MB1137 NUCLEO-H743ZI. D11 uses PA7 only with SB33 ON and SB35 OFF; changing
those solder bridges requires matching the SPI GPIO configuration.
