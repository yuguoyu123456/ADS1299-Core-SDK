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

For the ESP32-S3-DevKitC-1 reference path, all hardware-dependent values now
live in one obvious file:

`board/esp32s3_devkitc1_ads1299.h`

The default reference wiring is SCLK=GPIO12, MOSI=GPIO11, MISO=GPIO13,
CS=GPIO10, DRDY=GPIO9, RESET=GPIO8, START=GPIO7 and PWDN=GPIO6. When using a
different ESP32-S3 board, change this board header after checking that board's
schematic; do not edit the shared ADS1299 core.

## Repository layers

- ADS1299 behavior: `../../../core_driver/ads1299/`
- This platform's hardware-only adapter: `ads1299_port/`
- Native ESP-IDF HAL binding: `components/ads1299_esp_idf_hal/`
- Reference board configuration: `board/esp32s3_devkitc1_ads1299.h`
- Bounded stream queue: `main/ads1299_stream_queue.[ch]`
- Runnable ESP-IDF entrypoint: `main/main.c`
- Minimal portable call flow retained for reference: `examples/main_ads1299.c`
- Vendor-project procedure: `integration.md`
- Reproducible PlatformIO build: `platformio.ini` and `build.md`
- Official references and provenance: `sources.md`

The platform adapter never defines ADS1299 registers. UART, USB, BLE or Wi-Fi
transport must not block the DRDY/SPI acquisition path.

## Progressive beginner flow

The runnable `main/main.c` now performs these stages in order:

1. hardware reset and SDATAC;
2. ADS1299-family ID probe;
3. 250-SPS internal-test configuration and eight captured frames;
4. input-short configuration and eight captured frames;
5. 250-SPS EEG configuration with gain 24, normal input, SRB1 off and SRB2 off;
6. continuous RDATAC/START acquisition.

Continuous acquisition is event-driven: the DRDY falling edge wakes a
high-priority acquisition task, which reads one complete frame and pushes it to
a fixed 16-frame queue. A lower-priority transport task drains the queue. Queue
depth, high-watermark and dropped-frame counts are exposed so slow Wi-Fi/BLE/
UART work is visible instead of silently corrupting acquisition timing.

Replace only the sparse logging section in `transport_task()` with the desired
transport backend. Do not move network or formatting work into the acquisition
task or DRDY ISR.

## Build / flash / run

With ESP-IDF installed:

```bash
cd firmware/mcu/02_Espressif/ESP32S3
idf.py set-target esp32s3
idf.py build
idf.py -p <serial-port> flash monitor
```

Expected progress messages include a successful ADS1299 family probe,
`internal-test: 8 frames captured`, `input-short: 8 frames captured`, the EEG250
profile description, and `event-driven bounded EEG250 acquisition started`.
Queue-overflow diagnostics report `dropped` and `high` counts if transport falls
behind.

Host-only queue regression source is in `tests/test_stream_queue.c` and can be
built with:

```bash
cd firmware/mcu/02_Espressif/ESP32S3/tests
make -f Makefile.host clean
make -f Makefile.host test
```

## Validation boundary

The repository records an earlier clean ESP-IDF/PlatformIO compile for this
ESP32-S3 model; see `validation.md` and `build.md`. The new board-config,
progressive diagnostic and bounded event-driven streaming changes in the current
maintenance pass have **not** yet been re-build-verified in this environment.
They are not BOARD-VERIFIED, and no physical ADS1299 behavior is implied.

The pinned build uses PlatformIO's N8/no-PSRAM board definition. GPIO19/20 stay
free for native USB. Product UART/USB/BLE/Wi-Fi packetization is intentionally
kept outside the timing-critical acquisition path.
