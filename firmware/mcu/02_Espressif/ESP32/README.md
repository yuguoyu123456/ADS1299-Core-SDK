# ESP32-WROOM-32 ADS1299 Port

Global ecosystem rank: **14**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Espressif
- Family / MCU: ESP32 / ESP32-WROOM-32
- Architecture: Xtensa LX6 dual-core
- Reference board: ESP32-DevKitC V4 / 4 MB WROOM reference
- Official environment: ESP-IDF
- Compiler: Xtensa ESP ELF GCC

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

## Maintained ESP32 reference

Original IDF queued binding and DevKitC V4 WROOM board profile use SPI3, 1 MHz, GPIO18/23/19 with controls 21/22/25/26/27. Actual bootloader/application build and mock fault tests passed. See board/reference_image.md and build.md. Hardware qualification remains separate.
