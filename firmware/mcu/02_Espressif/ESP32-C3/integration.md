# Integration with ESP-IDF

1. Install the official `ESP-IDF` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `ESP32-C3` / `Select an official ESP32-C3 evaluation board`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c` and include path.
5. Implement `board_ads1299_hal()` using the vendor SDK's SPI, GPIO and microsecond-delay APIs.
6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.
7. Copy the call flow from `examples/main_ads1299.c`; keep product transport outside the example.
8. First verify power, ID read, register readback and internal test input. Only then connect electrodes.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware
stay in the user's official SDK project. This repository owns only the thin
callback adapter and ADS1299-independent tests. Pin `pin in the consuming official SDK project` in the
consumer project and record any API change in `version.md`.

## Current concrete ESP-IDF reference path

The generic integration notes above are retained for compatibility with custom
ESP32-C3 projects. New users should start from the repository's standalone
reference project instead:

```sh
cd examples/esp_idf_reference
idf.py set-target esp32c3
idf.py build
idf.py -p <serial-port> flash monitor
```

The documented reference board is **ESP32-C3-DevKitM-1**. Hardware-dependent
pins and SPI settings live in one board file:

```text
board/esp32c3_devkitm1_ads1299.h
```

The ESP-IDF adapter in `ads1299_port/ads1299_espidf_hal.c` owns only vendor SPI,
GPIO, delay and DRDY interrupt binding. ADS1299 register semantics remain in the
shared core.

For sustained EEG acquisition, DRDY is configured as a falling-edge GPIO
interrupt. The ISR performs no SPI, logging, packet formatting or radio work; it
only gives a FreeRTOS task notification. The higher-priority acquisition task
consumes one notification per pending DRDY edge, reads one ADS1299 frame, and
pushes it into the fixed 16-frame queue. A lower-priority transport task drains
the queue. This keeps Wi-Fi/BLE/UART work outside the DRDY timing path while
retaining explicit `dropped` and `high_watermark` diagnostics.

Diagnostic internal-test and input-short captures continue to use the shared
portable `ads1299_wait_drdy()` polling helper before the ISR is installed. The
interrupt-driven path is installed only for sustained EEG250 streaming.
