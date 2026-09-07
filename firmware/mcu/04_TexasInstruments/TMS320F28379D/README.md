# TMS320F28379D ADS1299 Port

Global ecosystem rank: **17**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Texas Instruments
- Family / MCU: C2000 / TMS320F28379D
- Architecture: dual C28x + dual CLA
- Reference board: LAUNCHXL-F28379D
- Official environment: C2000Ware DriverLib
- Compiler: TI C2000 compiler

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
