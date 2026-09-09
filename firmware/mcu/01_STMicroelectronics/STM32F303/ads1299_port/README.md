# STM32F303 ADS1299 port

This directory is the STM32F303 platform-adapter layer for the shared ADS1299 core in `firmware/core_driver/ads1299`.

It intentionally contains no ADS1299 register-policy logic. Board-specific STM32 HAL code should bind the callbacks declared by `ads1299_platform_hal_t`; the shared core then uses the resulting `ads1299_port_t`.

## Responsibilities

- SPI transfer callback forwarding
- CS, RESET, PWDN and START GPIO routing
- active-low DRDY interpretation
- microsecond delay forwarding
- construction of the portable `ads1299_port_t`

The hardware SPI peripheral must be configured as ADS1299 SPI Mode 1: CPOL=0, CPHA=1, 8-bit, MSB-first. Pin and peripheral choices belong in the model `board/` layer, not in this directory or the shared ADS1299 core.

## Host software self-test

From this directory, on a host with a C99 compiler:

```sh
make -f Makefile.host test
```

Expected successful output:

```text
PASS STM32F303 ads1299_port host self-test
```

The self-test uses a fake platform HAL and verifies callback routing, SPI TX/RX forwarding, CS/RESET/PWDN/START control, DRDY active-low semantics, delay forwarding, and invalid-argument guards. It does not require STM32 hardware.

## Validation status

- Host adapter self-test infrastructure: PRESENT
- STM32CubeIDE/reference-board build: NOT YET VERIFIED
- Physical STM32F303 + ADS1299 operation: NOT BOARD-VERIFIED

A host self-test pass validates software adapter behavior only. It does not validate pin assignments, SPI electrical timing, clocks, ADS1299 power/reference design, or real acquisition quality.
