# STM32F767 ADS1299 host tests

These tests exercise the STM32F767 model adapter together with the shared ADS1299 core without requiring STM32 hardware.

## What is covered

- 24-bit ADS1299 sample conversion boundary behavior.
- Opaque register access boundary behavior.
- STM32F767 SPI callback forwarding.
- RESET GPIO routing and delay forwarding.
- DRDY active-low semantics through both the MCU convenience API and portable `ads1299_port_t` callback.
- Canonical repository packet encode/validate/decode coverage using the shared `ads1299_packet` implementation.
- CRC16 corruption detection by flipping one payload bit and requiring validation failure.
- Link-time integration of `ads1299.c`, `ads1299_frame.c`, `ads1299_model.c`, `ads1299_multi.c`, and `firmware/common/data_packet/ads1299_packet.c` with the STM32F767 port sources.

## Run

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The build uses C99 plus `-Wall -Wextra -Werror -pedantic` by default.

Expected final line:

```text
PASS STM32F767 integration smoke tests
```

## Scope of this validation

A successful host run is software integration evidence only. It does **not** establish STM32Cube build verification or physical NUCLEO-F767ZI + ADS1299 board verification.

Current repository status for this test leaf:

- canonical packet/CRC regression: **PRESENT**
- shared packet source linkage: **PRESENT**
- current-revision host PASS recorded by this maintenance run: **NO**
- STM32 target BUILD-VERIFIED: **NO**
- NUCLEO-F767ZI + ADS1299 BOARD-VERIFIED: **NO**

The host target intentionally links the existing shared core and shared packet implementation rather than maintaining reduced platform-local copies. If a shared module develops an incompatible dependency, the failure should be fixed without removing an established core source merely to make this test pass.
