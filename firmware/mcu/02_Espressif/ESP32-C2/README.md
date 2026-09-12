# ESP32-C2 ADS1299 Port

Status: **TEMPLATE / PLANNED** for validation purposes. Source integration is substantially present, but the current revision is not yet recorded as BUILD-VERIFIED or BOARD-VERIFIED.

## Reference platform

- MCU family: ESP32-C2 / ESP8684
- Reference board: **Espressif ESP8684-DevKitM-1 v1.1**
- Toolchain / SDK: ESP-IDF
- Beginner board file: `board/esp8684_devkitm1_ads1299.h`
- Standalone project: `examples/esp_idf_reference/`

Espressif's current development-board documentation maps the ESP32-C2 series to ESP8684 and lists ESP8684-DevKitM-1 as an official development board.

## ADS1299 wiring used by the reference project

| ADS1299 | ESP8684-DevKitM-1 |
|---|---:|
| SCLK | GPIO6 |
| DIN / MOSI | GPIO7 |
| DOUT / MISO | GPIO2 |
| CS | GPIO10 |
| DRDY | GPIO3 |
| RESET | GPIO4 |
| PWDN | GPIO5 |
| START | GPIO18 |

SPI is configured as Mode 1 (CPOL=0, CPHA=1), MSB first, 4 MHz, with software-controlled CS. Confirm the exact board revision before wiring.

## What a beginner edits

For the documented reference board, do not edit `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared-core files. Hardware-dependent pins and the SPI clock are centralized in:

```text
board/esp8684_devkitm1_ads1299.h
```

When moving to another ESP32-C2 board, start by changing only that board layer. If the alternate board needs a different peripheral topology, adapt the ESP-IDF HAL rather than copying ADS1299 register logic.

## Build / flash / run

With ESP-IDF activated:

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/examples/esp_idf_reference
idf.py set-target esp32c2
idf.py build
idf.py -p <serial-port> flash monitor
```

The project links the shared ADS1299 core directly and runs this progressive flow:

```text
probe / ID
  -> internal test (8 frames)
  -> input short (8 frames)
  -> 250-SPS EEG configuration
  -> DRDY falling-edge wakeup
  -> high-priority acquisition task
  -> fixed 16-frame queue
  -> lower-priority transport task
```

Expected healthy output includes messages similar to:

```text
probe OK: ADS1299-family ID=0x.. channels=8
internal-test: capturing 8 frames
input-short: capturing 8 frames
250-SPS EEG profile ready: channels=8 gain=24 normal-input SRB1=off SRB2=off
DRDY falling-edge notification enabled on GPIO3
250-SPS EEG streaming started: event-driven DRDY, bounded queue=16 frames
beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream
```

A one-second DRDY timeout is reported separately from an SPI/frame-read failure. Queue overflow increments a `dropped` counter and preserves `high_watermark`; unread frames are not silently overwritten.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- ESP32-C2 SPI/GPIO/DRDY and ESP-IDF binding: `ads1299_port/`
- One board/config layer: `board/esp8684_devkitm1_ads1299.h`
- Bounded queue: `examples/esp32c2_frame_queue.[ch]`
- Runnable ESP-IDF project: `examples/esp_idf_reference/`
- Host queue regression: `tests/`
- Detailed integration notes: `integration.md`
- Validation boundary: `validation.md`

The sustained-acquisition ISR only wakes the acquisition task. SPI, logging, Wi-Fi/BLE work and packet transport do not execute in the ISR. Slow transport belongs on the lower-priority consumer side.

## Host regression

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/tests
make -f Makefile.host clean
make -f Makefile.host test
```

The test sources cover FIFO ordering, wraparound, overflow/drop accounting, invalid arguments and 8192-frame producer/consumer interleaving. These tests are source-present; this README does not claim they passed unless a run is recorded.

## Validation status

- Source/reference-board integration: **PRESENT**
- ESP-IDF HAL: **PRESENT**
- Probe/internal-test/input-short/250-SPS flow: **PRESENT**
- Event-driven bounded sustained acquisition: **PRESENT**
- Host regression source/build recipe: **PRESENT**
- Current revision BUILD-VERIFIED: **NOT YET RECORDED**
- BOARD-VERIFIED: **NOT YET RECORDED**

Do not infer bench testing, RF coexistence validation, long-run loss rate, electrical safety, EMC or production readiness from source completeness alone.
