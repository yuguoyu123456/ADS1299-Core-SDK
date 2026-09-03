# ESP32 / ESP32-S3 ESP-IDF example — source added, not yet toolchain-verified

Recommended first target: ESP32-S3.

Configure the SPI device with:
- mode = 1 (CPOL=0, CPHA=1)
- 8-bit transfers / MSB first
- `spics_io_num = -1` because the common driver controls CS explicitly
- conservative clock first; raise only after timing and signal-integrity checks

Use a GPIO interrupt on DRDY falling edge. For higher rates, keep acquisition separate from USB/Wi-Fi/BLE transport and avoid doing network work inside the DRDY ISR.
