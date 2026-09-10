# STM32G4 ADS1299 tests

This folder contains two complementary validation paths. They intentionally coexist.

## 1. Portable host integration smoke test

Use this first when changing the STM32G4 model-local port, shared ADS1299 core integration, or canonical packet transport.

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The host target compiles the STM32G4 model-local `ads1299_port` together with the repository shared ADS1299 sources and shared canonical packet implementation. The unified runner checks:

- signed 24-bit ADS1299 sample conversion boundaries;
- register-boundary assumptions already captured by the existing tests;
- SPI callback forwarding;
- RESET/control GPIO forwarding;
- active-low DRDY behavior through both the model helper and portable `ads1299_port_t` callback;
- canonical packet encode/validate/decode and CRC corruption detection.

Expected final line:

```text
PASS STM32G4 integration smoke tests
```

This is a software/host smoke test only. A pass does **not** imply STM32CubeG4 target build verification or physical NUCLEO-G474RE + ADS1299 board verification.

## 2. Genuine STM32G474/CMSIS polling adapter tests

The pre-existing `compile_adapter.py`, `run_polling_tests.py`, and `test_polling.c` exercise the lower-level STM32G474 adapter against real device/CMSIS headers when the required official toolchain/header trees are supplied. Keep this path separate from the portable host runner because it validates a different boundary.

Do not remove that test path merely because `Makefile.host` exists.

## Source ownership

Platform tests must consume, not duplicate, shared ADS1299 behavior:

- `../../../../core_driver/ads1299/` owns ADS1299 commands, registers, frames, model handling and multi-device behavior.
- `../../../../common/data_packet/` owns the canonical host packet and CRC implementation.
- `../ads1299_port/` owns STM32G4 model-local SPI/GPIO/DRDY adaptation.

A new student or platform maintainer should never edit `ads1299.c`, `ads1299_regs.h`, or `ads1299_model.c` merely to make these tests run.

## Validation status

- Test sources: PRESENT.
- Portable host build recipe: PRESENT.
- Genuine-header polling test path: PRESENT (pre-existing).
- Host PASS recorded in repository: NOT YET.
- STM32CubeG4 BUILD-VERIFIED: NOT CLAIMED.
- NUCLEO-G474RE + ADS1299 BOARD-VERIFIED: NOT CLAIMED.
- Sustained DMA/EXTI acquisition and 64-channel operation: NOT VERIFIED by these tests.
