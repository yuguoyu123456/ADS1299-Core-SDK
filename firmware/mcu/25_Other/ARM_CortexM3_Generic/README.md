# Cortex-M3 ADS1299 Port

Global ecosystem rank: **94**. Status: **Reference**. Tier C. Hardware validation is not implied.

## Platform

- Vendor: ARM Generic
- Family / MCU: Cortex-M3 / Cortex-M3
- Architecture: Arm Cortex-M3
- Reference board: Select an official Cortex-M3 evaluation board
- Official environment: CMSIS device pack supplied by silicon vendor
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
