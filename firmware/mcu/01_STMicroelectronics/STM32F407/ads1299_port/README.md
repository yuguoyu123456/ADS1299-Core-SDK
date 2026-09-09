# STM32F407 ADS1299 port

This folder contains the STM32F407 platform adapter for the shared portable `ads1299_port_t` contract.

## Scope

The files in this folder only translate platform callbacks into the shared ADS1299 port interface. ADS1299 register policy, profiles, frame decoding, model handling and multi-device behavior remain in `firmware/core_driver/ads1299/`.

The adapter expects a platform HAL with:

- full-duplex SPI transfer;
- CS, RESET, PWDN and START output control;
- DRDY input readback;
- microsecond delay callback.

`DRDY` is active-low. `ads1299_mcu_data_ready()` therefore reports true when the physical pin reads low, while `ads1299_port_t.drdy_read()` preserves the raw pin level required by the shared port contract.

## Host self-test

No STM32 HAL headers are required for this test. From this directory run:

```sh
make -f Makefile.host test
```

The target is compiled with strict C99 warnings:

```text
-std=c99 -Wall -Wextra -Werror -pedantic
```

The test checks callback forwarding, SPI transfer validation, CS/RESET/PWDN/START routing, active-low DRDY semantics and delay forwarding.

Expected successful output:

```text
PASS STM32F407 ads1299_port host self-test
```

## Validation status

- Port implementation present: YES
- Host self-test infrastructure present: YES
- Host test pass recorded in repository: NO
- STM32Cube build verified: NO
- Physical STM32F407 + ADS1299 board verified: NO

Do not interpret host-test success as hardware validation.
