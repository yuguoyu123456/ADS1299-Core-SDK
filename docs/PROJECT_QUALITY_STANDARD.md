# Project quality standard

This repository is not considered a vendor SDK merely because a platform folder exists. Every controller family must eventually provide a **real, reproducible project**.

## Definition of a complete controller project

A platform is `Project-complete` only when the repository contains, for a representative board:

1. a build-system entry point (CMake/Make/PlatformIO/vendor IDE project as appropriate),
2. startup and board configuration needed by that build system,
3. explicit pin mapping for SCLK/MOSI/MISO/CS/DRDY/RESET/START,
4. SPI Mode 1 configuration (CPOL=0, CPHA=1), MSB first,
5. ADS1299 reset and device-ID read,
6. register read/write/readback demonstration,
7. internal-test-signal configuration,
8. DRDY-driven 27-byte frame acquisition,
9. signed 24-bit channel conversion,
10. UART/USB/other host transport using the common packet format,
11. error handling and counters for timeouts/dropped frames,
12. a README with exact toolchain version, wiring, build/flash commands and expected output,
13. automated compilation where the toolchain can legally/reliably run in GitHub Actions,
14. real-board validation before the status can become `Bench-tested`.

A README-only folder, a port adapter without a `main`, or pseudocode is **not a project**.

## Required status vocabulary

- `Scaffold` — folder/notes only; not claimed as usable.
- `Source-complete` — all project source/config files are present but compilation has not been independently reproduced.
- `CI-build` — clean GitHub Actions build passes from a fresh environment.
- `Bench-tested` — real ADS1299-Core hardware passes the bring-up checklist.
- `24h-tested` — long-duration acquisition evidence is attached.

## Driver quality bar

The common driver is expected to grow into a readable, documented implementation covering the full ADS1299 programming model relevant to an 8-channel core module: commands, complete register field definitions, channel setup, SRB1/SRB2, BIAS sensing/drive controls, lead-off configuration/status, GPIO, internal test, input-short noise test, data-rate control, clock/daisy options, register dumps, frame/status decoding, conversion helpers, safe streaming state transitions and diagnostics.

Public projects such as OpenBCI are used as functional references, but code is not copied wholesale. The TI ADS1299 datasheet remains the primary device specification.
