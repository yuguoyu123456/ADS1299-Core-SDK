# STM32F072 host integration smoke tests

These tests exercise the repository-owned STM32F072 ADS1299 port together with the shared ADS1299 core on a normal desktop C compiler. They are intended to catch API drift and adapter regressions before a CubeIDE board build.

## Run

From this directory:

```sh
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32F072 integration smoke tests
```

The runner covers:

- ADS1299 24-bit signed sample boundary decoding through the shared core;
- proof that the STM32F072 platform layer treats register traffic as opaque bytes rather than duplicating ADS1299 register policy;
- SPI callback forwarding and TX/RX behavior;
- RESET GPIO routing and microsecond/millisecond delay conversion;
- DRDY active-low behavior, including the distinction between the MCU helper (`1` means data ready) and the shared `ads1299_port_t.drdy_read` electrical-level callback (`0` means the ADS1299 DRDY pin is asserted).

The build intentionally compiles the current repository copies of:

```text
firmware/core_driver/ads1299/ads1299.c
firmware/core_driver/ads1299/ads1299_frame.c
firmware/core_driver/ads1299/ads1299_model.c
firmware/core_driver/ads1299/ads1299_multi.c
firmware/mcu/01_STMicroelectronics/STM32F072/ads1299_port/*.c
```

This makes the test useful as a shared-core/platform integration smoke test rather than an isolated mock-only test.

## What this does not validate

Passing these host tests does **not** mean the STM32CubeIDE project has been compiled, flashed, or electrically validated. It does not validate SPI clock timing on a physical board, ADS1299 power/reference/clock wiring, analog performance, EMC, medical safety, or production readiness.

For real-board bring-up, use the STM32F072 model Quick Start and keep SPI configured as ADS1299 Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first.
