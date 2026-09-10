# STM32G4 ADS1299 port

This directory adapts STM32G4 board/HAL functions to the shared portable `ads1299_port_t` interface. ADS1299 register definitions, profiles, framing, multi-device behavior and high-level control stay in `firmware/core_driver/ads1299/`; do not duplicate them here.

## Responsibilities

- `ads1299_spi.c/.h`: forwards portable SPI transactions to the platform callback.
- `ads1299_gpio.c/.h`: maps CS, RESET, PWDN and START to platform GPIO callbacks and provides delays.
- `ads1299_drdy.c/.h`: exposes ADS1299 DRDY active-low semantics.
- `ads1299_port_selftest.c`: host-only callback/port smoke test with no STM32 hardware dependency.
- `Makefile.host`: strict host build for the self-test.

The platform owner supplies only the hardware-dependent callbacks in `ads1299_platform_hal_t`. A beginner should not edit shared ADS1299 core files to bring up STM32G4.

## Host smoke test

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The build uses C99 plus `-Wall -Wextra -Werror -pedantic`. Expected final line:

```text
PASS STM32G4 ads1299_port host self-test
```

This test checks callback forwarding, invalid SPI arguments, control-pin routing, active-low DRDY interpretation, portable `ads1299_port_t` construction, and microsecond/millisecond delay forwarding. It does not prove STM32G4 target compilation or physical ADS1299 operation.

## Acquisition architecture

Use the blocking path only for first bring-up. For continuous EEG, keep the DRDY interrupt path short and move each raw frame into a static bounded queue/ring buffer before packetization or host transport. Count queue overflow explicitly. DMA is appropriate when the selected STM32G4 peripheral/pin configuration supports it; any DMA implementation must preserve frame boundaries and deterministic CS handling.

## Validation status

- Port source: PRESENT.
- Host self-test source/build recipe: PRESENT.
- Host test execution: not recorded by this file.
- STM32G4 target BUILD-VERIFIED: not claimed.
- Physical board + ADS1299 BOARD-VERIFIED: not claimed.
