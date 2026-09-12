# ESP32-C6 ADS1299 Port

Global ecosystem rank: **13**. Status: **Reference**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: Espressif
- Family / MCU: ESP32-C6 / ESP32-C6
- Architecture: 32-bit RISC-V
- Reference board: ESP32-C6-DevKitC-1
- Official environment: ESP-IDF
- Compiler: riscv32-esp-elf-gcc

## ADS1299 connection

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep CS software-controlled and
route DRDY, RESET, PWDN and START as independent GPIOs. Start at 4 MHz or less
until ID read, configuration readback and the internal test signal pass. The
reference pin assignment is documented in `board/pinmap.md`; confirm it against
the exact board revision before wiring.

## Repository layers

- ADS1299 behavior: `../../../core_driver/ads1299/`
- This platform's hardware-only adapter: `ads1299_port/`
- ESP-IDF hardware binding: `esp_idf_adapter/` and `board/esp_idf_board.c`
- Progressive reference flow: `examples/main_ads1299.c`
- Bounded stream queue: `examples/esp32c6_frame_queue.[ch]`
- Vendor-project procedure: `integration.md`

The port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also
provides a millisecond helper without changing the stable Core port contract.
It never defines ADS1299 registers. UART, USB, BLE, Wi-Fi or Thread transport
must stay outside the DRDY/SPI acquisition path.

## Maintained ESP-IDF reference

Use the local `platformio.ini` / CMake entry, `esp_idf_adapter/` and
`board/esp_idf_board.c`. GPIO18/19 replace boot-strap GPIO4/5. See
`board/reference_image.md`, `sources.md` and `build.md`.

The maintained executable now performs the beginner sequence without requiring
shared-core edits:

1. initialize the ESP-IDF board/HAL and portable ADS1299 port;
2. hardware reset and enter SDATAC;
3. probe and validate an ADS1299-family ID;
4. run eight frames of the shared internal-test configuration;
5. stop cleanly, run eight frames of the shared input-short configuration;
6. configure 250 SPS, gain 24, normal electrode inputs and SRB1 off;
7. start RDATAC/START continuous acquisition;
8. run a higher-priority DRDY/SPI acquisition task that pushes complete frames
   into the bounded queue;
9. run a lower-priority transport task that drains the queue and performs only
   sparse logging by default.

The old `ads1299_latest_frame` / `ads1299_frame_sequence` debugger snapshot is
still maintained for compatibility, but transport no longer depends on it.

## Bounded continuous acquisition

`examples/esp32c6_frame_queue.[ch]` provides a fixed 16-frame, zero-heap queue.
Each record carries the decoded `ads1299_frame_t`, a microsecond timestamp and a
monotonic sequence number. Queue depth, high-watermark and dropped-frame
counters make transport back-pressure visible rather than silently overwriting
unconsumed EEG data.

The reference task split is:

```text
DRDY -> SPI frame read -> bounded queue -> UART/Wi-Fi/BLE/Thread/log consumer
       acquisition task                  transport task
       higher priority                   lower priority
```

Replace the sparse log block in `transport_task()` with the desired packet
transport. Do not move network, BLE, console formatting or other blocking work
into `acquisition_task()`.

## Host regression

Host regression source is available under `tests/` and can be run with:

```sh
cd firmware/mcu/02_Espressif/ESP32C6/tests
make -f Makefile.host clean
make -f Makefile.host test
```

The regression covers FIFO ordering, wraparound, explicit overflow/drop
accounting, invalid arguments and an 8192-frame producer/consumer interleave.
Presence of these sources does **not** imply the command has been executed in CI
or on hardware.

## Validation status

- Source/integration for ESP-IDF: **PRESENT**.
- Probe -> internal-test -> input-short -> 250-SPS EEG source flow: **PRESENT**.
- Bounded acquisition/transport task split: **PRESENT**.
- Host queue regression source: **PRESENT**.
- Host regression execution in this repository/CI: **NOT YET RECORDED**.
- ESP-IDF reference build: **NOT YET BUILD-VERIFIED**.
- Physical ESP32-C6-DevKitC-1 + ADS1299 run: **NOT BOARD-VERIFIED**.

Do not interpret source presence as bench, long-run, electrical-safety or
production-readiness validation.
