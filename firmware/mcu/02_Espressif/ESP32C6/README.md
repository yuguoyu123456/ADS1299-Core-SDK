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
- Minimal call flow: `examples/main_ads1299.c`
- Vendor-project procedure: `integration.md`

The port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also
provides a millisecond helper without changing the stable Core port contract.
It never defines ADS1299 registers. UART, USB, BLE or Ethernet transport stays
in `firmware/transport/` and must not block a DRDY handler.

## Maintained ESP-IDF reference

Use the local platformio.ini / CMake entry, esp_idf_adapter/ and board/esp_idf_board.c. GPIO18/19 replace boot-strap GPIO4/5. See board/reference_image.md, sources.md and build.md. Single-task dedicated SPI2 with persistent timeout buffers; latest-frame debugger output only.

## Bounded acquisition building block

`examples/esp32c6_frame_queue.[ch]` adds a fixed 16-frame, zero-heap queue for decoupling future DRDY/SPI acquisition work from slower UART/Wi-Fi/BLE/Thread consumers. Each record carries the decoded `ads1299_frame_t`, a timestamp and a sequence number. Queue depth, high-watermark and dropped-frame counters are explicit so transport back-pressure is observable rather than silent. The queue source is compiled by the maintained ESP-IDF reference build; integration into a dedicated acquisition/transport task split remains the next platform step.

Host regression source is available under `tests/` and can be run with:

```sh
cd firmware/mcu/02_Espressif/ESP32C6/tests
make -f Makefile.host clean
make -f Makefile.host test
```

The regression covers FIFO ordering, wraparound, explicit overflow/drop accounting, invalid arguments and an 8192-frame producer/consumer interleave. Presence of these sources does **not** imply the command has been executed in CI or on hardware. Current validation remains source/integration present; ESP-IDF BUILD-VERIFIED and BOARD-VERIFIED are not claimed.
