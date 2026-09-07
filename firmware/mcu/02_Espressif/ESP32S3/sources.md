# Official sources and provenance

| Scope | Official source | Use |
|---|---|---|
| MCU | [ESP32-S3 datasheet v2.2](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf) | CPU, SRAM, peripherals, USB and electrical planning |
| Board | [ESP32-S3-DevKitC-1 v1.0 guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.0.html) | J1 header numbering, USB pins and board variants |
| SPI | [ESP-IDF SPI master guide](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/peripherals/spi_master.html) | SPI2/SPI3 ownership and master-driver API |
| SDK | [ESP-IDF v5.4](https://github.com/espressif/esp-idf/tree/v5.4) | GPIO, SPI and ROM-delay APIs |
| Build | [PlatformIO Espressif32 6.10.0](https://github.com/platformio/platform-espressif32/releases/tag/v6.10.0) | Reproducible ESP-IDF 5.4.0 toolchain integration |
| AFE | [TI ADS1299 datasheet SBAS499C](https://www.ti.com/lit/ds/symlink/ads1299.pdf) | ADS1299 timing, commands, registers and SPI Mode 1 |

ESP-IDF and PlatformIO's Espressif platform are Apache-2.0 licensed. They are
installed outside this repository and are not redistributed here. The HAL
binding was independently written around this repository's callback contract.

The checked PlatformIO board manifest models an 8 MiB N8 board without PSRAM.
Espressif's physical DevKitC-1 ordering codes vary, so users must confirm their
module marking and board revision before copying memory settings or wiring.
