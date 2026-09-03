# ESP32-S3 DevKitC — native ESP-IDF reference project

Status: **Source-complete; CI build tracked. Hardware validation pending.**

This project is a native ESP-IDF reference design for ADS1299 acquisition. The ADS1299 platform-adaptation layer lives inside this project under `components/ads1299_port/`, while the common device driver and packet encoder remain shared across the repository.

## Project contents

- `main/main.c` — board initialization, ADS1299 bring-up and acquisition loop.
- `components/ads1299_port/` — ESP-IDF SPI/GPIO adaptation.
- `../../../core_driver/ads1299/` — shared controller-independent ADS1299 driver.
- `../../../common/data_packet/` — shared binary packet encoder.
- `CMakeLists.txt` — complete ESP-IDF project configuration.
- `sdkconfig.defaults` — reproducible project defaults.

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

SPI2 is configured as a 4 MHz Mode-1 master with software-controlled CS. DRDY is a falling-edge GPIO interrupt. UART0 outputs the same 49-byte packet used by the other reference projects.

## Build

From the repository root:

```bash
cd firmware/mcu/espressif/esp32s3_devkitc
idf.py set-target esp32s3
idf.py build
idf.py -p YOUR_PORT flash monitor
```

## First boot profile

The firmware starts with the ADS1299 internal test source at 250 SPS so the digital path can be verified before electrodes are attached.

Do not switch to human EEG until these steps pass:

1. power / reset check;
2. ADS1299 ID read;
3. register access;
4. internal-test waveform on all 8 channels;
5. packet sequence / CRC verification on the host.

## Real-time design rule

Wi-Fi or BLE work must not run inside the DRDY ISR. Wireless transport should use a buffered producer/consumer design so RF and network jitter cannot silently disturb ADS1299 acquisition timing.
