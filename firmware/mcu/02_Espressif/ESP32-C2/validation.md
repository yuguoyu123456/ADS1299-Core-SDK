# Validation

Current status: **TEMPLATE / PLANNED**

The ESP32-C2 folder now contains a concrete ESP-IDF adapter, a reference-board
configuration for ESP8684-DevKitM-1 v1.1, a standalone ESP-IDF project, and a
bounded sustained-acquisition queue path.  These files are present in source,
but no clean target compile/link or physical-board run is claimed yet.

Software-side evidence now present:

- ESP-IDF SPI/GPIO binding exists and preserves the portable ADS1299 callback contract;
- the reference application performs reset -> SDATAC -> ID/probe -> internal-test -> RDATAC/START;
- acquisition and transport are separated into different FreeRTOS tasks;
- a fixed 16-frame queue prevents slow UART/Wi-Fi/BLE work from blocking the DRDY/SPI path;
- queue diagnostics expose `dropped` and `high_watermark` rather than silently overwriting data;
- host regression sources cover FIFO order, wraparound, overflow accounting, invalid arguments,
  and 8192-frame producer/consumer interleaving;
- `tests/Makefile.host` provides a standalone host build for the queue regression.

Not yet established:

- **BUILD-VERIFIED**: `idf.py build` has not been recorded as successful for the documented reference configuration;
- **BOARD-VERIFIED**: no physical ESP8684-DevKitM-1 + ADS1299 run is recorded;
- no long-run RF coexistence, electrical safety, EMC, production-readiness or 24-hour loss-rate claim is made.

Next evidence: clean host regression pass -> clean ESP-IDF target build -> ADS1299 ID/internal-test capture on the stated board -> sustained 250-SPS acquisition with queue/drop accounting -> transport coexistence testing.
