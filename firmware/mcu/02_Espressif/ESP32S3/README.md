# ESP32-S3 ADS1299 Port

Global ecosystem rank: **3**. Status: **Compiles**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Espressif
- Family / MCU: ESP32-S3 / ESP32-S3
- Architecture: Xtensa LX7 dual-core
- Reference board: ESP32-S3-DevKitC-1-N8
- Official environment: ESP-IDF
- Compiler: xtensa-esp-elf-gcc 14.2.0

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


- Native ESP-IDF HAL binding: `components/ads1299_esp_idf_hal/`
- Reproducible PlatformIO build: `platformio.ini` and `build.md`
- Official references and provenance: `sources.md`

The pinned build uses PlatformIO's N8/no-PSRAM board definition. GPIO19/20 stay
free for native USB. Product UART/USB/BLE/Wi-Fi packetization is intentionally
absent from `main/main.c` and belongs in `firmware/transport/`.
