# STM32U575ZI ADS1299 Port

Global ecosystem rank: **12**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32U5 / STM32U575ZI
- Architecture: Arm Cortex-M33
- Reference board: NUCLEO-U575ZI-Q; provisional control GPIOs
- Official environment: STM32CubeU5
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

## Maintained U575 reference

Original cmsis_adapter/ targets U575 SPI1 with packet-counted transfers, bounded waits and an ES0499 EOT guard. Reference link and modeled fault/chunk tests passed. Use board/reference_image.c only with TrustZoneDisabled reset MSI4 clocks. See build.md, board/reference_image.md and sources.md. No physical boot or secure/nonsecure deployment is qualified.
