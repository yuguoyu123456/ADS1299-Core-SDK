# STM32H723 ADS1299 host integration tests

This directory provides a host-side smoke-test build for the STM32H723 ADS1299 adapter. It is intended to catch portable integration regressions without claiming target-board validation.

Run from this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The runner includes the pre-existing frame, register-boundary, SPI and RESET tests and adds DRDY active-low behavior plus the repository canonical packet encode/validate/decode and CRC-corruption regression. The build links the STM32H723 port, the shared ADS1299 core (`ads1299.c`, `ads1299_frame.c`, `ads1299_model.c`, `ads1299_multi.c`) and `firmware/common/data_packet/ads1299_packet.c`; platform-local copies of register or packet logic are intentionally not used.

Expected successful final line:

```text
PASS STM32H723 integration smoke tests
```

Validation status: test sources and host build recipe are PRESENT. A successful host execution for the current revision is NOT recorded here. This does not establish STM32CubeH7 target BUILD-VERIFIED status, physical NUCLEO-H723ZG + ADS1299 BOARD-VERIFIED status, DMA/cache correctness, electrical performance, sustained-throughput validation, or production readiness.
