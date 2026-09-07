# nRF5340 application core ADS1299 Port

Global ecosystem rank: **7**. Status: **Compiles**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Nordic Semiconductor
- Family / MCU: nRF5340 / nRF5340 application core
- Architecture: dual Arm Cortex-M33
- Reference board: nRF5340 DK
- Official environment: nRF Connect SDK / Zephyr
- Compiler: arm-none-eabi-gcc 9.2.1

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

## Verified hardware-SPI application

The current CMake entry uses zephyr_adapter/, the standard Port and the minimal example. See zephyr_adapter/README.md and build.md. Legacy src/board_sdk.c and port/ files are retained but not built. Output is a latest-frame debugger snapshot, not UART/USB/BLE streaming.
