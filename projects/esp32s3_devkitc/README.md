# ESP32-S3 DevKitC — native ESP-IDF reference project

Status: **Source-complete; CI build pending on this branch. Hardware validation pending.**

## Pin map

| ADS1299-Core | ESP32-S3 |
|---|---|
| SCLK | GPIO12 |
| DIN/MOSI | GPIO11 |
| DOUT/MISO | GPIO13 |
| CS | GPIO10 |
| DRDY | GPIO9 |
| RESET | GPIO8 |
| START | GPIO7 |

SPI2 is configured as a 4 MHz Mode-1 master with software-controlled CS. DRDY is a falling-edge GPIO interrupt. UART0 outputs the same 49-byte packet used by the STM32 reference project.

## Build

Install ESP-IDF, then:

```bash
cd projects/esp32s3_devkitc
idf.py set-target esp32s3
idf.py build
idf.py -p YOUR_PORT flash monitor
```

The first firmware profile uses the ADS1299 internal test source at 250 SPS. Do not change to electrodes until ID/readback/internal-test validation passes.

The design intentionally keeps acquisition in a tight loop and does not perform Wi-Fi/BLE work in the DRDY ISR. Wireless transport will be added as a separate buffered layer so RF/network jitter cannot silently corrupt acquisition timing.
