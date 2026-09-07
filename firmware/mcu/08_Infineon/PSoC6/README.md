# CY8C624ABZI-S2D44 ADS1299 Port

Global ecosystem rank: **10**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Infineon
- Family / MCU: PSoC6 / CY8C624ABZI-S2D44
- Architecture: Arm Cortex-M4F + Cortex-M0+
- Reference board: CY8CKIT-062S2-43012
- Official environment: ModusToolbox HAL
- Compiler: GCC Arm Embedded

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

## Maintained PDL reference

The original pdl_adapter/ uses nonblocking PDL FIFO APIs with bounded polling. See build.md and board/reference_image.md. A 28-source reference links with the official prebuilt CM0P_SLEEP image; modeled fault tests passed. Control GPIO assignments are provisional, not verified header positions. Output is a debugger snapshot, not a lossless stream.
