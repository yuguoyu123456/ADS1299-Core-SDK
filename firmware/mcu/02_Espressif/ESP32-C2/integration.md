# Integration with ESP-IDF

1. Install the official `ESP-IDF` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `ESP32-C2` / `Select an official ESP32-C2 evaluation board`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c` and include path.
5. Implement `board_ads1299_hal()` using the vendor SDK's SPI, GPIO and microsecond-delay APIs when using a custom board.
6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.
7. Keep transport work outside the DRDY/SPI path; use the bounded queue pattern from the ESP-IDF reference project.
8. First verify power, ID read, register readback and internal test input. Only then connect electrodes.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware
stay in the user's official SDK project. This repository owns only the thin
callback adapter and ADS1299-independent tests. Pin the consuming official SDK
project to a known ESP-IDF version and record API changes in `version.md`.

## Concrete reference path: ESP8684-DevKitM-1 v1.1

A real ESP-IDF binding is provided for the official Espressif
**ESP8684-DevKitM-1 v1.1** reference board:

- edit board pins/clock only in `board/esp8684_devkitm1_ads1299.h`;
- ESP-IDF SPI/GPIO callbacks are in `ads1299_port/ads1299_espidf_hal.c`;
- a standalone reference project is in `examples/esp_idf_reference/`;
- `examples/esp32c2_frame_queue.[ch]` provides the fixed-capacity sustained-acquisition queue;
- the example links the repository shared ADS1299 core directly instead of copying register logic into the ESP32-C2 folder.

The default reference assignment is SCLK=GPIO6, MOSI=GPIO7, MISO=GPIO2,
CS=GPIO10, DRDY=GPIO3, RESET=GPIO4, PWDN=GPIO5 and START=GPIO18. It avoids
GPIO8/GPIO9 (ESP8684 strapping pins), GPIO19/GPIO20 (UART0) and GPIO0/GPIO1
(RGB LED channels on board revision v1.1). Re-check the exact board revision
before wiring.

From an activated ESP-IDF 5.2.x environment:

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/examples/esp_idf_reference
idf.py set-target esp32c2
idf.py build
idf.py -p <serial-port> flash monitor
```

Expected successful bring-up messages now include:

```text
probe OK: ADS1299-family ID=0x..
internal-test streaming started: bounded queue=16 frames
acquisition runs above transport priority; slow UART/Wi-Fi/BLE work must stay in transport task
frame seq=0 t=...us status=...... ch1=...
```

The reference application uses two FreeRTOS tasks:

```text
higher-priority acquisition task
    DRDY -> complete ADS1299 SPI frame -> bounded queue
                                      |
                                      v
lower-priority transport task -> log / UART / Wi-Fi / BLE / packet transport
```

Queue-full events increment `dropped` and expose `high_watermark`; they never
silently overwrite unread EEG frames. Keep actual Wi-Fi/BLE/UART formatting,
packetization and socket work on the transport side.

Host-only queue regression can be built independently:

```sh
cd firmware/mcu/02_Espressif/ESP32-C2/tests
make -f Makefile.host clean
make -f Makefile.host test
```

Validation status: **TEMPLATE / PLANNED** until those builds are actually run
and recorded. Source presence does not imply BUILD-VERIFIED or BOARD-VERIFIED.
