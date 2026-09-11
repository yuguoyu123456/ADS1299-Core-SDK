# STM32C031 integration smoke tests

This directory checks the boundary between the shared ADS1299 core and the STM32C031 port without requiring a physical board.

## Run on a host PC

From this directory:

```sh
make -f Makefile.host test
```

The build intentionally compiles the current shared core together with the STM32C031 `ads1299_port` sources and these tests. This helps catch API drift between the shared driver and this model folder.

Expected final line:

```text
PASS STM32C031 integration smoke tests
```

## What is covered

- 24-bit signed sample boundaries used by ADS1299 frame decoding;
- confirmation that register behavior stays in the shared core instead of being reimplemented in this platform folder;
- SPI transfer/write/read routing through `ads1299_mcu_port_t`;
- RESET GPIO routing and microsecond/millisecond delay forwarding.

Additional integration coverage now includes:

- ADS1299 DRDY active-low semantics through both the STM32C031 helper and portable `ads1299_port_t.drdy_read()` callback;
- canonical repository packet encode/validate/decode using the shared `firmware/common/data_packet/ads1299_packet.c` implementation;
- canonical sync bytes, version/flags, sequence, timestamp, three status bytes and eight signed channel values;
- CRC protection by deliberately corrupting a payload bit and requiring validation failure.

The packet test intentionally links the shared packet implementation instead of copying packet/CRC logic into this model folder. The host build therefore exercises STM32C031 port sources, shared ADS1299 core sources and the canonical transport packet contract together.

These are host-side software smoke tests only. A PASS does not mean the STM32C031 reference board, ADS1299 wiring, analog performance, electrical safety, EMC, or long-run acquisition has been hardware-verified.

For board bring-up, use the model-root Quick Start and the runnable example under `../examples`.
