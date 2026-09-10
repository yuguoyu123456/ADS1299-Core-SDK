# STM32H533 ADS1299 tests

This leaf contains host-runnable smoke tests for the STM32H533 ADS1299 platform layer and its integration with the repository shared core. The tests do not claim STM32CubeH5 target-build or physical-board validation.

## What is covered

- 24-bit ADS1299 sample sign-extension boundaries through the shared core.
- Register ownership boundary: platform code transports bytes but does not redefine ADS1299 register logic.
- STM32H533 portable SPI adapter callback forwarding.
- RESET/control-line adapter behavior retained by the existing tests.
- DRDY active-low semantics in both the STM32H533 helper and portable `ads1299_port_t` callback.
- Canonical packet encode/validate/decode round trip, including CRC corruption rejection.
- Link integration of the STM32H533 port with `ads1299.c`, frame/model/multi-device modules, and the canonical packet module.

## Run on a host

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32H533 integration smoke tests
```

The host target uses a normal C99 compiler and fake platform callbacks. It does not require STM32CubeH5 headers or hardware.

## Validation status

- Test source: PRESENT.
- Host build recipe: PRESENT.
- Host execution result in repository validation records: NOT YET RECORDED.
- STM32CubeH5 / NUCLEO-H533RE target build: NOT CLAIMED BUILD-VERIFIED here.
- NUCLEO-H533RE + ADS1299 physical hardware: NOT CLAIMED BOARD-VERIFIED here.
- Sustained EXTI/DMA acquisition, long-run transport, and multi-ADS1299/64-channel hardware behavior: NOT VERIFIED by these host tests.

Use the model-level `board/board_config.h` and `examples/` for bring-up; beginners should not edit shared ADS1299 core files for these tests or normal board integration.
