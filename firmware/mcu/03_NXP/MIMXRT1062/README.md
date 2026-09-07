# MIMXRT1062DVL6B ADS1299 Port

Global ecosystem rank: **5**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: NXP
- Family / MCU: i.MX RT1060 / MIMXRT1062DVL6B
- Architecture: Arm Cortex-M7F
- Reference board: MIMXRT1060-EVKB
- Official environment: MCUXpresso SDK
- Compiler: arm-none-eabi-gcc / MCUXpresso IDE

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

## Maintained reference

This package targets **MIMXRT1060-EVKB** revision B-class hardware and its
MIMXRT1062DVL6B device. The repository-owned MCUXpresso adapter exposes only
LPSPI/GPIO/delay behavior; all ADS1299 protocol and register work stays in Core.

The J17/J16 mapping is taken from the NXP EVKB manual and current MCUXpresso
LPSPI example. Generate GPIO and LPSPI mux code in the consuming SDK project.
