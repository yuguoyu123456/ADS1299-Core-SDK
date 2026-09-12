# Validation

Current status: **TEMPLATE / PLANNED**

Present in-tree:
- ESP32-H2-DevKitM-1 board configuration for ADS1299 wiring;
- ESP-IDF SPI/GPIO/delay binding through the portable ADS1299 port layer;
- standalone ESP-IDF reference project linked to the shared ADS1299 core;
- progressive probe -> internal-test -> input-short -> 250-SPS EEG flow;
- 16-frame static bounded acquisition queue with sequence/timestamp metadata;
- queue depth, high-watermark and dropped-frame diagnostics;
- host regression source for FIFO order, wraparound, overflow accounting, invalid arguments and an 8192-frame producer/consumer interleaving run.

Host queue test command:

```sh
cd firmware/mcu/02_Espressif/ESP32-H2/tests
make -f Makefile.host clean
make -f Makefile.host test
```

Expected successful host runner output:

```text
ESP32-H2 bounded frame queue tests: PASS
```

Validation boundary:
- The host regression source and Makefile are present, but this repository state is not labelled HOST-PASS solely because files exist.
- A local compile/run attempt from the maintenance environment could not reach `github.com`, so no fresh host execution result is claimed here.
- No ESP-IDF BUILD-VERIFIED claim is made until `idf.py build` succeeds for the documented ESP32-H2 target/reference configuration.
- No BOARD-VERIFIED claim is made until the stated physical board + ADS1299 hardware completes probe, internal-test, input-short, 250-SPS streaming, deliberate transport back-pressure/drop accounting and clean stop/SDATAC checks.
- No long-run, production, EMC, electrical-safety or regulatory claim is made.
