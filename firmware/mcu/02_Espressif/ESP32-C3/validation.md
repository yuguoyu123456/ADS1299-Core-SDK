# Validation

## Current status

**TEMPLATE / PLANNED for target hardware validation.**

The ESP32-C3 folder is no longer only a directory scaffold. It now contains:

- an ESP32-C3-DevKitM-1 board configuration;
- an ESP-IDF SPI/GPIO HAL binding;
- a standalone ESP-IDF reference project that links the shared ADS1299 core;
- progressive probe -> internal-test -> input-short -> 250-SPS EEG flow;
- a bounded 16-frame acquisition queue with drop/high-watermark diagnostics;
- host regression source for FIFO ordering, wraparound, overflow accounting,
  argument validation, and an 8192-frame producer/consumer interleaving run.

These source files and build recipes are **present**, but source presence is not a
validation pass.

## Host queue regression

From this model folder:

```sh
cd tests
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line on success:

```text
ESP32-C3 bounded frame queue tests: PASS
```

The host regression exercises software queue behavior only. It does not exercise
ESP-IDF, SPI electrical timing, GPIO routing, ADS1299 DRDY timing, Wi-Fi/BLE
contention, or physical hardware.

## Target build validation still required

A **BUILD-VERIFIED** claim requires an actual successful build of the documented
reference project with the documented ESP-IDF toolchain, for example:

```sh
cd examples/esp_idf_reference
idf.py set-target esp32c3
idf.py build
```

No BUILD-VERIFIED claim is made until that command (or an equivalent CI target)
has actually completed successfully against the repository revision being
reported.

## Board validation still required

A **BOARD-VERIFIED** claim requires running the built image on the stated physical
ESP32-C3-DevKitM-1 + ADS1299 hardware and recording evidence for at least:

1. ADS1299 family ID/probe success;
2. internal-test frame capture;
3. input-short frame capture;
4. 250-SPS EEG continuous acquisition;
5. bounded-queue overflow/drop diagnostics under deliberately slowed transport;
6. clean continuous stop/SDATAC behavior.

No bench, long-run, electrical-safety, EMC, production-readiness, or regulatory
claim is made.
