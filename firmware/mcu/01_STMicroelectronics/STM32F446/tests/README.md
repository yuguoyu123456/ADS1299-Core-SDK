# STM32F446 ADS1299 host smoke tests

Status: **HOST TEST INFRASTRUCTURE PRESENT**. These tests validate portable C integration on a host compiler. They do **not** prove STM32Cube build success or physical NUCLEO-F446RE + ADS1299 operation.

## What this leaf validates

The test runner covers:

- shared-core 24-bit sample sign-extension boundary;
- the rule that the STM32F446 port transports opaque ADS1299 register bytes instead of redefining register policy;
- SPI callback forwarding through `ads1299_port_t`;
- RESET GPIO and delay forwarding;
- DRDY active-low behavior at both MCU convenience and portable-port levels;
- canonical repository packet encode/validate/decode using the shared `ads1299_packet` implementation;
- CRC rejection after a deliberate payload bit flip;
- link compatibility between the STM32F446 port sources, the existing shared `ads1299.c`, `ads1299_frame.c`, `ads1299_model.c`, `ads1299_multi.c` modules, and `firmware/common/data_packet/ads1299_packet.c`.

## Run

From this directory on a machine with a C99 compiler and `make`:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32F446 integration smoke tests
```

## Interpretation

A host PASS means the tested platform/core/packet interfaces compile, link and behave as asserted under the host fakes. It does not validate SPI electrical timing, GPIO routing, CubeMX configuration, DRDY interrupt latency, sustained streaming, DMA behavior, or physical ADS1299 hardware.

Current repository status for this leaf: packet/CRC test source and shared packet linkage are present; a host PASS for the current repository state is not claimed here unless a run is separately recorded. STM32 target BUILD-VERIFIED and NUCLEO-F446RE + ADS1299 BOARD-VERIFIED are also not claimed.

For board bring-up use the sibling `board/`, `ads1299_port/`, and `examples/` folders. A normal beginner should change board/config and generated vendor-HAL routing only; shared ADS1299 core files are not a board configuration surface.
