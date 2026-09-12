# Validation

Current status: **TEMPLATE / PLANNED**

The ESP32-P4 folder now contains a concrete ESP32-P4-Function-EV-Board v1.4
configuration, an ESP-IDF SPI/GPIO binding, a standalone ESP-IDF reference
project, progressive ADS1299 probe/internal-test/input-short/250-SPS EEG source,
and a bounded sustained-acquisition queue.

## Source/integration evidence present

- Board/config header with explicit wiring and SPI Mode 1 at 4 MHz.
- ESP-IDF HAL adapter that preserves the portable null-TX clocking contract.
- Shared ADS1299 core linked into the reference project; register logic is not copied.
- Probe and ADS1299-family ID validation.
- Internal-test and input-short diagnostic capture.
- 250-SPS, gain-24, normal-input configuration using shared typed APIs.
- Continuous RDATAC/START acquisition separated from lower-priority transport.
- 32-frame bounded queue with count, high-watermark and dropped-frame diagnostics.
- Host regression source covering FIFO, wraparound, overflow and a 16,384-frame interleaving run.

## Evidence not yet established

- **HOST TEST PASS:** not recorded in this repository yet.
- **BUILD-VERIFIED:** no successful documented ESP-IDF `idf.py build` result for this reference configuration yet.
- **BOARD-VERIFIED:** no recorded physical ESP32-P4-Function-EV-Board + ADS1299 run yet.
- No claim of DMA optimization, FPGA-style timing closure, 24-hour hardware run,
  electrical safety, EMC, production readiness or regulatory validation.

## Commands to establish the next evidence level

Host queue regression:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/tests
make -f Makefile.host clean
make -f Makefile.host test
```

ESP-IDF build:

```bash
cd firmware/mcu/02_Espressif/ESP32-P4/examples/esp_idf_reference
idf.py set-target esp32p4
idf.py build
```

A future BOARD-VERIFIED record should include at least: ID/probe, internal-test,
input-short, 250-SPS EEG frames, deliberate transport back-pressure with drop
accounting, and clean STOP/SDATAC behavior.
