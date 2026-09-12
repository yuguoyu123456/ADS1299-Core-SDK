# Integration with ESP-IDF

## Reference path

Reference board: **Espressif ESP32-P4-Function-EV-Board v1.4**.

The concrete project is already in:

`examples/esp_idf_reference/`

Build and run with the official ESP-IDF environment:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/examples/esp_idf_reference
idf.py set-target esp32p4
idf.py build
idf.py -p <serial-port> flash monitor
```

The reference project links the shared ADS1299 sources directly from
`firmware/core_driver/ads1299/`; it does not duplicate register logic.

## Beginner-editable hardware file

For the reference board, hardware-dependent choices are centralized in:

`board/esp32p4_function_ev_ads1299.h`

Default wiring:

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

When adapting another ESP32-P4 board, change this board/config header first.
Do not edit `ads1299.c`, `ads1299_regs.h`, or `ads1299_model.c` for pin changes.

## Runnable sequence

The reference application executes:

1. ESP-IDF HAL initialization.
2. Shared-core `ads1299_init()`.
3. Hardware reset and SDATAC.
4. ADS1299-family ID/probe.
5. Internal-test profile and eight diagnostic frames.
6. Input-short profile and eight diagnostic frames.
7. 250-SPS EEG configuration, gain 24, normal electrode input.
8. RDATAC + START.
9. High-priority acquisition task -> 32-frame bounded queue.
10. Lower-priority transport/log task with queue depth, high-watermark and drop diagnostics.

Expected successful monitor output includes messages such as:

```text
probe OK: ADS1299-family ID=0x.. channels=8
internal-test frame=0 ...
input-short frame=0 ...
beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream
EEG250 seq=0 ...
```

## Timing and transport boundary

The ADS1299 acquisition path does not perform product networking or bulk logging.
The acquisition task only checks DRDY, reads the complete frame, timestamps it,
and pushes it into a bounded queue. Slow UART/USB/network work belongs in a
lower-priority consumer. Queue overflow is explicit through `dropped`, current
`count`, and `high_watermark` diagnostics.

The current reference HAL intentionally starts with deterministic polling SPI at
4 MHz and software-controlled CS. This is suitable for bring-up and 250-SPS EEG.
ESP32-P4 DMA optimization can be layered into the platform HAL later without
changing the shared ADS1299 core API or beginner board configuration contract.

## Host queue regression

Without hardware, the bounded queue can be exercised with:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/tests
make -f Makefile.host clean
make -f Makefile.host test
```

This covers FIFO ordering, wraparound, explicit overflow/drop accounting,
argument validation, and a 16,384-frame producer/consumer interleaving run.
Source/test presence is not itself a recorded PASS until the command is actually
executed in a suitable environment.

## SDK ownership boundary

Vendor startup files, linker scripts, ESP-IDF internals, generated configuration
and middleware remain owned by the official ESP-IDF project. This repository owns
the ADS1299 shared core, thin ESP32-P4 adapter, board config, examples and tests.
