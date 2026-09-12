# ESP32-WROOM-32 ADS1299 Port

Global ecosystem rank: **14**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Reference platform

- MCU/module: classic ESP32 / ESP32-WROOM-32
- Reference board: ESP32-DevKitC V4 / 4 MB WROOM reference
- Toolchain: ESP-IDF / Xtensa ESP ELF GCC
- One board-dependent file: `board/esp32_devkitc_v4_ads1299.h`

Default reference wiring:

| ADS1299 | ESP32 GPIO |
|---|---:|
| SCLK | 18 |
| DOUT / MISO | 19 |
| DIN / MOSI | 23 |
| CS | 21 |
| RESET | 22 |
| PWDN | 25 |
| START | 26 |
| DRDY | 27 |

The reference uses SPI3/VSPI at 1 MHz, SPI Mode 1 (CPOL=0, CPHA=1), MSB first,
and software-controlled CS. A different classic-ESP32 board should normally need
only board/config changes; do not edit the shared ADS1299 core for pin changes.

## Beginner path

The executable flow in `examples/main_ads1299.c` is now:

1. initialize the ESP-IDF board/HAL and portable `ads1299_port_t`;
2. hardware reset + SDATAC;
3. read and validate the ADS1299-family ID;
4. run internal-test at 250 SPS / gain 24 and capture 8 frames;
5. run input-short validation and capture 8 frames;
6. configure 250-SPS EEG, gain 24, normal input, SRB1/SRB2 off;
7. start continuous acquisition into a fixed 16-frame queue;
8. consume frames in a lower-priority transport task.

The queue records frame, sequence and microsecond timestamp and exposes depth,
high-watermark and dropped-frame counters. Slow UART/Wi-Fi/BLE work belongs in
the transport task, not in the acquisition path. The legacy
`ads1299_latest_frame` debugger snapshot remains available for compatibility.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- Portable MCU adapter: `ads1299_port/`
- ESP-IDF HAL binding: `esp_idf_adapter/`
- Reference board/config: `board/esp32_devkitc_v4_ads1299.h`
- Progressive reference flow: `examples/main_ads1299.c`
- Bounded queue: `examples/esp32_frame_queue.[ch]`
- Queue host regression: `tests/test_frame_queue.c`
- Integration notes: `integration.md`

Host queue regression source can be built with:

```sh
cd firmware/mcu/02_Espressif/ESP32/tests
make -f Makefile.host clean
make -f Makefile.host test
```

Expected successful test output is `ESP32 bounded frame queue tests: PASS`.

## Validation status

A historical 2026-09-07 ESP-IDF 5.4.0 software build baseline is recorded in
`build.md`; that baseline predates the new board-header/progressive-flow/bounded-
queue changes. Therefore the current source additions are **not yet reclassified
as BUILD-VERIFIED** until the updated project is rebuilt. No BOARD-VERIFIED claim
is made. The new queue regression source is present, but a host PASS is only valid
after the command above is actually executed successfully.

For another classic ESP32 board, edit the board/config layer first. If bring-up
requires changing `ads1299.c`, `ads1299_regs.h` or other shared core files, treat
that as an integration defect rather than a normal beginner step.
