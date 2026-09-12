# Validation

Current status: **TEMPLATE / PLANNED**

The ESP32-C5 folder now contains a concrete ESP-IDF adapter, an ESP32-C5-DevKitC-1 board configuration, a standalone ESP-IDF reference project, progressive probe/internal-test/input-short/250-SPS EEG source, and a bounded sustained-acquisition queue. These source/integration assets do **not** by themselves establish BUILD-VERIFIED or BOARD-VERIFIED status.

## Host-verifiable queue coverage

`tests/test_frame_queue.c` exercises FIFO ordering, wraparound, explicit overflow/drop accounting, argument validation, and an 8192-frame producer/consumer interleaving run. The queue is fixed-capacity/static and exposes `dropped` plus `high_watermark` diagnostics so transport back-pressure is observable rather than silently overwriting unread EEG frames.

Run from this folder:

```sh
cd firmware/mcu/02_Espressif/ESP32-C5/tests
make -f Makefile.host clean
make -f Makefile.host test
```

A source file and Makefile being present is not a recorded test pass; only report HOST PASS after the command has actually completed successfully in a validation environment.

## Sustained ESP-IDF acquisition path

The reference app separates timing-sensitive acquisition from slower transport/log work:

`DRDY -> complete ADS1299 SPI frame -> 16-frame bounded queue -> lower-priority transport task`

Queue access is protected by a FreeRTOS critical section. Overflow increments `dropped` and transport reports both dropped count and high-water mark. This is intended to keep future Wi-Fi/BLE/UART work out of the DRDY/SPI timing path.

## Evidence still required

- **BUILD-VERIFIED:** successful `idf.py set-target esp32c5 && idf.py build` using the documented ESP-IDF/reference configuration.
- **BOARD-VERIFIED:** physical ESP32-C5-DevKitC-1 + ADS1299 evidence for ID/probe, internal test, input short, 250-SPS EEG, sustained streaming, intentional transport back-pressure/drop accounting, and clean STOP/SDATAC.
- No 24-hour, electrical-safety, EMC, production-readiness, or regulatory claim is made.
