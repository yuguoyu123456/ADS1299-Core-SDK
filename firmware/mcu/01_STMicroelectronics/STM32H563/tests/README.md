# STM32H563 ADS1299 tests

This folder contains host-side smoke tests for the STM32H563 ADS1299 integration layer. These tests do not require STM32CubeH5 or physical hardware; they exercise the model-local port together with the repository shared ADS1299 core and canonical packet implementation.

## What is covered

The unified runner covers:

- signed 24-bit sample conversion boundaries;
- the register/core ownership boundary;
- SPI callback forwarding and invalid-argument handling;
- RESET/control GPIO routing;
- ADS1299 DRDY active-low semantics through both the STM32H563 helper and portable `ads1299_port_t` callback;
- canonical packet encode/validate/decode, including CRC corruption detection.

The existing individual tests are intentionally preserved. `test_main.c` only provides a single integration runner around them and the new DRDY/packet tests.

## Host build

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The host target links the STM32H563 port with these shared repository modules:

- `core_driver/ads1299/ads1299.c`
- `core_driver/ads1299/ads1299_frame.c`
- `core_driver/ads1299/ads1299_model.c`
- `core_driver/ads1299/ads1299_multi.c`
- `common/data_packet/ads1299_packet.c`

Expected final line on success:

```text
PASS STM32H563 integration smoke tests
```

## Validation status

- Test source completeness: PRESENT for the current integration-smoke scope.
- Shared-core/packet host build recipe: PRESENT.
- Host execution result: not recorded by this file; only claim PASS after actually running the command above.
- STM32CubeH5 target build: NOT BUILD-VERIFIED by these host tests.
- NUCLEO-H563ZI + ADS1299 hardware: NOT BOARD-VERIFIED by these host tests.
- Sustained EXTI/DMA acquisition, queue overflow accounting and long-run streaming: not validated here.

These tests intentionally do not duplicate ADS1299 register configuration inside the platform folder. Shared ADS1299 behavior remains owned by the common core.
