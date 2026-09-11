# STM32F429 ADS1299 host smoke tests

Status: **HOST TEST INFRASTRUCTURE PRESENT**. These tests validate portable C integration on a host compiler. They do **not** prove STM32Cube build success or physical STM32F429I-DISC1 + ADS1299 operation.

## What this leaf validates

The test runner covers:

- shared-core 24-bit sample sign extension boundary;
- the rule that the STM32F429 port transports opaque ADS1299 register bytes instead of redefining register policy;
- SPI callback forwarding through `ads1299_port_t`;
- RESET GPIO and delay forwarding;
- DRDY active-low behavior at both MCU convenience and portable-port levels;
- canonical ADS1299 packet encode/validate/decode through the shared `firmware/common/data_packet/ads1299_packet.c` implementation;
- canonical packet fields including sync, version/flags, sequence, timestamp, 3 status bytes, and 8 signed channel values;
- CRC rejection after a deliberate one-bit payload corruption;
- link compatibility between the STM32F429 port sources and the existing shared `ads1299.c`, `ads1299_frame.c`, `ads1299_model.c`, `ads1299_multi.c`, and canonical packet modules.

## Run

From this directory on a machine with a C99 compiler and `make`:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32F429 integration smoke tests
```

## Interpretation

A host PASS means the tested platform/core/packet interfaces compile, link and behave as asserted under the host fakes. It does not validate SPI electrical timing, GPIO routing, CubeMX configuration, DRDY interrupt latency, sustained streaming, DMA behavior, physical ADS1299 hardware, analog performance, electrical safety, EMC, or production readiness.

For board bring-up use the sibling `board/`, `ads1299_port/`, and `examples/` folders. A normal beginner should change board/config and generated vendor-HAL routing only; shared ADS1299 core files are not a board configuration surface.

## Validation status

- Host smoke-test sources: **PRESENT**
- Canonical packet/CRC regression: **PRESENT**
- Shared packet linkage: **PRESENT**
- Host test PASS recorded for this repository state: **NO**
- STM32 target BUILD-VERIFIED: **NO**
- STM32F429I-DISC1 + ADS1299 BOARD-VERIFIED: **NO**
