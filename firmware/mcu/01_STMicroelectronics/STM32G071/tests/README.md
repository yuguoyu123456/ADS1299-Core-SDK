# STM32G071 ADS1299 host tests

This folder provides host-side smoke coverage for the STM32G071 ADS1299 integration without requiring an MCU board or ADS1299 hardware.

## What is covered

- signed 24-bit ADS1299 sample/frame boundary handling;
- register-boundary expectations already used by this model;
- STM32G071 SPI callback forwarding;
- RESET/control-line forwarding and delay behavior;
- DRDY active-low semantics through both the MCU convenience API and portable `ads1299_port_t`;
- canonical 49-byte packet encode/decode/CRC validation using the shared `firmware/common/data_packet` implementation;
- link-level integration against the shared ADS1299 core (`ads1299.c`, frame, model and multi-device modules).

The tests intentionally do not duplicate ADS1299 register logic inside this model folder.

## Run

From this directory on a host with a C99 compiler and `make`:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32G071 integration smoke tests
```

The Makefile uses `-std=c99 -Wall -Wextra -Werror -pedantic` and resolves shared sources from `firmware/core_driver/ads1299` and `firmware/common/data_packet`.

## Validation status

- Test source: PRESENT.
- Host build recipe: PRESENT.
- Host execution PASS: not recorded by this file; only mark it after an actual successful run.
- STM32Cube/NUCLEO-G071RB build verification: not implied.
- Physical NUCLEO-G071RB + ADS1299 validation: not implied.

These tests are software smoke tests only. They do not prove SPI electrical timing, DRDY interrupt latency, sustained acquisition, multi-ADS1299 operation, or 64-channel hardware behavior.
