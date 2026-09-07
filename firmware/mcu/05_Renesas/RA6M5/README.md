# R7FA6M5BH3CFC ADS1299 Port

Global ecosystem rank: **8**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Renesas
- Family / MCU: RA6 / R7FA6M5BH3CFC
- Architecture: Arm Cortex-M33
- Reference board: EK-RA6M5
- Official environment: Renesas FSP
- Compiler: Arm GNU 13.2 recommended by FSP; adapter test GCC 9.2.1

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

## FSP reference build

The original board/reference_image.c links with external FSP 5.6 startup/BSP and TinyUSB 0.18 configuration. See board/reference_image.md and build.md. Start at 1 MHz. API fault-path tests and complete reference ELF link passed; physical routing and hardware acquisition remain unverified. For a generated project use the alternative board/fsp_binding.c, never both bindings. Output is a debugger snapshot, not a lossless stream.
