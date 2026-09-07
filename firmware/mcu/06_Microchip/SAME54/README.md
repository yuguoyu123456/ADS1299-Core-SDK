# ATSAME54P20A ADS1299 Port

Global ecosystem rank: **9**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Microchip
- Family / MCU: SAME5x / ATSAME54P20A
- Architecture: Arm Cortex-M4F
- Reference board: SAME54 Xplained Pro
- Official environment: MPLAB Harmony 3
- Compiler: XC32 / arm-none-eabi-gcc

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

## Maintained SAME54 reference

Use harmony_adapter/ and the clock-specific board/reference_image.c. See build.md and board/reference_image.md for the complete GNU reference link and provisional control GPIOs. The SPI adapter uses official DFP definitions, not a copied PLIB. Start at 1 MHz; do not share SERCOM6 with vendor SPI ISR/DMA. Output is a debugger snapshot only.
