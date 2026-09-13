# Validation

Current status: **Compiles** — the catalog lifecycle label reflects recorded software build evidence; the newer board-config, progressive diagnostics and bounded-streaming revision still requires a fresh reference build before that newer revision is called BUILD-VERIFIED.

## Current status detail

- Historical platform baseline: **BUILD-VERIFIED** for the clean ESP-IDF/PlatformIO build recorded on 2026-09-04.
- Current board-config + progressive diagnostic + bounded streaming changes: **TEMPLATE / PLANNED until re-build-verified**.
- Physical hardware status: **not BOARD-VERIFIED**.

The earlier clean build used ESP-IDF 5.4.0 and Xtensa GCC 14.2.0 with:

`platformio run -d firmware/mcu/02_Espressif/ESP32S3`

See `build.md` for the recorded command and memory result. That evidence proves
compilation/linking of the earlier baseline only; it does not automatically
validate later source changes.

## Current source/integration coverage

The current ESP32-S3 model folder contains:

- ESP32-S3-DevKitC-1 board/config header;
- native ESP-IDF HAL binding and portable `ads1299_port_t` bridge;
- ADS1299 family ID probe;
- internal-test capture;
- input-short capture;
- 250-SPS EEG configuration with gain 24 and normal inputs;
- DRDY falling-edge notification to a high-priority acquisition task;
- fixed 16-frame bounded queue between acquisition and transport;
- queue depth, high-watermark and dropped-frame diagnostics;
- host regression source covering FIFO, overflow, wraparound, invalid arguments
  and 8192-frame producer/consumer interleaving.

The host regression entrypoint is:

```bash
cd firmware/mcu/02_Espressif/ESP32S3/tests
make -f Makefile.host clean
make -f Makefile.host test
```

The repository-wide Espressif host matrix now exercises this entrypoint. Treat
that host regression independently from native ESP-IDF build verification and
from physical BOARD-VERIFIED status.

## Evidence required for stronger claims

To mark the current revision BUILD-VERIFIED, attach the exact ESP-IDF/compiler
version plus a clean `idf.py build` or documented PlatformIO build log from the
current commit.

To mark it BOARD-VERIFIED, additionally run the stated ESP32-S3-DevKitC-1 with
ADS1299 hardware and retain evidence for ID/probe, internal test, input-short,
250-SPS EEG acquisition, bounded-stream overflow accounting and clean stop.

No bench, long-run, electrical-safety, EMC, production-readiness or regulatory
claim is made by this file.
