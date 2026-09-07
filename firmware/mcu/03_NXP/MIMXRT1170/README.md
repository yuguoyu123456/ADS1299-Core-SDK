# MIMXRT1176DVMAA ADS1299 Port

Global ecosystem rank: **4**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: NXP
- Family / MCU: i.MX RT1170 / MIMXRT1176DVMAA
- Architecture: dual Arm Cortex-M7/M4
- Reference board: MIMXRT1170-EVKB
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

This package targets the **MIMXRT1170-EVKB**, its MIMXRT1176DVMAA device and
the Cortex-M7 image. `mcuxpresso_adapter/` converts NXP LPSPI/GPIO calls into
the repository hardware-only HAL contract; ADS1299 commands and registers
remain exclusively in Core.

The LPSPI1 route follows NXP's current EVKB example. Control signals use
Arduino D2-D5; generate their exact GPIO mux with MCUXpresso Config Tools for
the installed board revision. Do not add an SDK, startup or linker files here.
