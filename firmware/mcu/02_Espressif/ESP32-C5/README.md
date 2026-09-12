# ESP32-C5 ADS1299 Port

Global ecosystem rank: **123**. Status: **TEMPLATE / PLANNED (concrete ESP-IDF integration present; build and board verification still pending)**. Tier C.

## Recommended reference path

- MCU: ESP32-C5
- Reference board: **Espressif ESP32-C5-DevKitC-1 v1.2**
- Toolchain / SDK: ESP-IDF with ESP32-C5 target support
- Beginner-editable hardware file: `board/esp32c5_devkitc1_ads1299.h`
- Runnable reference project: `examples/esp_idf_reference/`

The reference board is an Espressif board based on ESP32-C5-WROOM-1(U). The selected ADS1299 pins are broken out on the v1.2 headers and avoid the documented ESP32-C5 strapping pins and the native USB GPIO13/GPIO14 pair.

## Reference wiring

| ADS1299 | ESP32-C5-DevKitC-1 |
|---|---:|
| SCLK | GPIO6 |
| DIN / MOSI | GPIO8 |
| DOUT / MISO | GPIO9 |
| CS | GPIO10 |
| DRDY | GPIO4 |
| RESET | GPIO5 |
| PWDN | GPIO23 |
| START | GPIO24 |
| GND | GND |

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. The reference HAL starts at 4 MHz and keeps CS software-controlled. Verify the exact board revision/header labels before wiring.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- ESP32-C5 SPI/GPIO/DRDY adapter: `ads1299_port/`
- Real ESP-IDF HAL binding: `ads1299_port/ads1299_espidf_hal.[ch]`
- Board-only configuration: `board/esp32c5_devkitc1_ads1299.h`
- Portable generic example retained for compatibility: `examples/main_ads1299.c`
- Standalone ESP-IDF reference project: `examples/esp_idf_reference/`

A new student should normally change only the board header when adapting another ESP32-C5 board. Do not edit `ads1299.c`, `ads1299_regs.h` or the shared model layer for ordinary board bring-up.

## Build / flash / run

From an ESP-IDF shell with ESP32-C5 target support:

```bash
cd firmware/mcu/02_Espressif/ESP32-C5/examples/esp_idf_reference
idf.py set-target esp32c5
idf.py build
idf.py -p <serial-port> flash monitor
```

The reference project directly builds the existing shared `ads1299.c`, `ads1299_frame.c`, `ads1299_model.c` and `ads1299_multi.c` together with the ESP32-C5 port. It does not duplicate ADS1299 register configuration in the platform folder.

## Beginner diagnostic flow

The current executable reference flow is:

1. initialize the ESP-IDF SPI/GPIO HAL;
2. initialize the portable `ads1299_port_t` bridge;
3. hardware RESET and SDATAC;
4. read and validate the ADS1299-family ID;
5. run the shared internal-test configuration and capture 8 frames;
6. run the shared input-short configuration and capture 8 frames;
7. configure normal-input, gain-24, **250-SPS EEG** and capture 8 frames;
8. STOP + SDATAC cleanly.

Expected successful messages include text similar to:

```text
probe OK: ADS1299-family ID=0x.. channels=8
internal-test frame=0 status=...... ch1=...
input-short frame=0 status=...... ch1=...
EEG250 frame=0 status=...... ch1=...
beginner diagnostic flow complete: probe -> internal-test -> input-short -> EEG250
```

Errors separately identify HAL/port initialization, RESET/SDATAC, probe/ID, configuration, DRDY timeout and frame-read failures.

## Continuous streaming status

Continuous Wi-Fi/BLE/UART streaming is **not yet enabled in this ESP32-C5 reference project**. The diagnostic example intentionally stops after finite EEG250 capture. Before adding a radio/network transport, add a bounded acquisition queue so DRDY/SPI work is not blocked by Wi-Fi, BLE, logging or other slow transport activity.

## Validation status

- Source-level board configuration: **PRESENT**
- ESP-IDF SPI/GPIO HAL binding: **PRESENT**
- Shared-core build integration metadata: **PRESENT**
- Probe / internal-test / input-short / EEG250 executable source: **PRESENT**
- ESP-IDF build actually executed successfully for this reference configuration: **NOT YET RECORDED**
- Physical ESP32-C5-DevKitC-1 + ADS1299 board verification: **NOT RECORDED**
- Continuous wireless bounded-stream validation: **NOT IMPLEMENTED YET**

Therefore this folder remains **TEMPLATE / PLANNED**, not BUILD-VERIFIED or BOARD-VERIFIED.

[Espressif ESP32-C5-DevKitC-1 documentation](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c5/esp32-c5-devkitc-1/user_guide.html) · [101–200 ecosystem list](../../ECOSYSTEM_101_200.md)
