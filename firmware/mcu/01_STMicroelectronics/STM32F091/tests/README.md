# STM32F091 host integration smoke tests

These tests exercise the repository-owned STM32F091 ADS1299 port together with the shared ADS1299 core and shared canonical packet codec on a normal desktop C compiler. They are intended to catch API drift, adapter regressions, and transport-contract regressions before a CubeIDE board build.

## Run

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32F091 integration smoke tests
```

The runner covers:

- ADS1299 24-bit signed sample boundary decoding through the shared core;
- proof that the STM32F091 platform layer treats register traffic as opaque bytes rather than duplicating ADS1299 register policy;
- SPI callback forwarding and TX/RX behavior;
- RESET GPIO routing and microsecond/millisecond delay conversion;
- DRDY active-low behavior, including the distinction between the MCU helper (`1` means data ready) and the shared `ads1299_port_t.drdy_read` electrical-level callback (`0` means the ADS1299 DRDY pin is asserted);
- canonical ADS1299 packet encode/validate/decode round trip using the shared `firmware/common/data_packet` codec;
- sync bytes, version, flags, sequence, timestamp, three status bytes, eight signed channel samples and CRC protection;
- deliberate payload-bit corruption to prove CRC validation rejects a damaged packet.

The build intentionally compiles the current repository copies of:

```text
firmware/core_driver/ads1299/ads1299.c
firmware/core_driver/ads1299/ads1299_frame.c
firmware/core_driver/ads1299/ads1299_model.c
firmware/core_driver/ads1299/ads1299_multi.c
firmware/common/data_packet/ads1299_packet.c
firmware/mcu/01_STMicroelectronics/STM32F091/ads1299_port/ads1299_spi.c
firmware/mcu/01_STMicroelectronics/STM32F091/ads1299_port/ads1299_gpio.c
firmware/mcu/01_STMicroelectronics/STM32F091/ads1299_port/ads1299_drdy.c
```

This makes the test useful as a shared-core/platform/packet integration smoke test rather than an isolated mock-only test. The packet test consumes the shared packet implementation; it does not define a competing STM32F091 wire format.

## What this does not validate

Passing these host tests does **not** mean the STM32CubeIDE project has been compiled, flashed, or electrically validated. It does not validate SPI clock timing on a physical board, ADS1299 power/reference/clock wiring, DRDY interrupt latency, long-run buffering, analog performance, EMC, medical safety, or production readiness.

For real-board bring-up, use the STM32F091 model Quick Start and keep SPI configured as ADS1299 Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first. Continuous acquisition should keep the DRDY path short and move packetization/host transport outside the timing-critical interrupt path.
