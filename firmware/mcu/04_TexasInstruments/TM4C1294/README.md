# TM4C1294NCPDT ADS1299 Port

Global ecosystem rank: **16**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Texas Instruments
- Family / MCU: TM4C129x / TM4C1294NCPDT
- Architecture: Arm Cortex-M4F
- Reference board: EK-TM4C1294XL
- Official environment: TivaWare
- Compiler: GCC Arm Embedded 9.2.1; CCS/TI Arm Clang not run

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

## Maintained SSI reference

Use tivaware_adapter/ with shared Core. SPI mode 1 legacy SSI0 uses PA2 SCK, PA4 TX and PA5 RX; do not copy the opposite TM4C123 mapping. Board controls are provisional and protected by a review interlock. See board/reference_image.md and build.md.
