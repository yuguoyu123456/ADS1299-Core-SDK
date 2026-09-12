# ESP32-P4 ADS1299 Port

Global ecosystem rank: **125**. Status: **TEMPLATE / PLANNED**. Tier C. Hardware validation is not implied.

## Reference platform

- MCU: ESP32-P4
- Reference board: **Espressif ESP32-P4-Function-EV-Board v1.4**
- Toolchain / SDK: official ESP-IDF
- Shared ADS1299 implementation: `../../../core_driver/ads1299/`
- Hardware adapter: `ads1299_port/`
- Beginner board config: `board/esp32p4_function_ev_ads1299.h`
- Runnable reference project: `examples/esp_idf_reference/`

The board configuration is the normal place to change pins, SPI host and SPI clock.
A beginner should not edit `ads1299.c`, `ads1299_regs.h` or `ads1299_model.c` to bring up a board.

## Default ADS1299 wiring

| ADS1299 | ESP32-P4-Function-EV-Board v1.4 |
|---|---|
| SCLK | GPIO4 |
| DIN / MOSI | GPIO5 |
| DOUT / MISO | GPIO3 |
| CS | GPIO7 |
| DRDY | GPIO8 |
| RESET | GPIO20 |
| PWDN | GPIO21 |
| START | GPIO22 |
| GND | GND |

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. The reference starts at 4 MHz with software-controlled CS.

## Quick Start

With ESP-IDF installed:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/examples/esp_idf_reference
idf.py set-target esp32p4
idf.py build
idf.py -p <serial-port> flash monitor
```

Expected progression when the hardware is connected correctly:

```text
probe OK: ADS1299-family ID=0x.. channels=...
internal-test frame=0 ...
input-short frame=0 ...
beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream
EEG250 seq=0 ...
```

The example uses shared typed ADS1299 APIs for internal test, input short, 250-SPS data rate, gain 24, normal electrode input, RDATAC/START and STOP/SDATAC. No register-byte setup is duplicated in this folder.

## Sustained acquisition architecture

ESP32-P4 uses a fixed **32-frame bounded queue** between acquisition and transport work:

```text
DRDY -> complete SPI frame -> timestamp/sequence -> bounded queue -> lower-priority transport
```

The queue reports current depth, high-watermark and dropped-frame count. Slow UART/USB/network work belongs on the consumer side, not in the DRDY/SPI path. The current bring-up HAL intentionally uses deterministic polling SPI; future DMA optimization must preserve the same shared-core and board-config contracts.

## Host regression

Queue logic can be exercised independently of hardware:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/tests
make -f Makefile.host clean
make -f Makefile.host test
```

The regression covers FIFO order, wraparound, explicit overflow accounting, invalid arguments and a 16,384-frame producer/consumer interleaving run.

## Validation status

- Reference board/config: **PRESENT**
- ESP-IDF HAL source: **PRESENT**
- Shared-core build integration: **PRESENT**
- Probe/internal-test/input-short/250-SPS source flow: **PRESENT**
- Bounded sustained-acquisition source: **PRESENT**
- Host regression source: **PRESENT**
- HOST TEST PASS: **not yet recorded**
- ESP-IDF BUILD-VERIFIED: **not yet established**
- BOARD-VERIFIED: **not yet established**

Do not interpret source presence as bench validation, production readiness, electrical safety, EMC, regulatory validation or long-run hardware testing.

See `integration.md` for the full integration path and `validation.md` for evidence boundaries.
