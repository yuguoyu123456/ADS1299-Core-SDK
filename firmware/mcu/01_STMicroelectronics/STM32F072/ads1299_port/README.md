# STM32F072 ADS1299 port layer

This directory adapts the shared `firmware/core_driver/ads1299` portable `ads1299_port_t` interface to STM32F072 board/HAL callbacks. ADS1299 register behavior belongs in the shared core; board-specific SPI and GPIO details belong outside that core.

## What this layer provides

- SPI transfer forwarding through `ads1299_platform_hal_t::spi_transfer`.
- CS, RESET, PWDN and START output routing.
- Active-low DRDY interpretation.
- Microsecond delay forwarding.
- Conversion into the repository-wide `ads1299_port_t` interface.

The vendor-specific STM32 HAL implementation should populate one `ads1299_platform_hal_t` instance. A beginner changing to another STM32F072 board should change the board/HAL binding rather than `ads1299.c`, `ads1299_regs.h` or the other shared ADS1299 core files.

## Host smoke test

The adapter can be tested without an STM32 board. From this directory run:

```sh
make -f Makefile.host test
```

The test compiles the real adapter sources against the shared `ads1299_port.h` and a fake platform HAL. It verifies SPI forwarding, CS/RESET/PWDN/START routing, active-low DRDY semantics, delays and basic invalid-argument guards.

Expected result:

```text
PASS STM32F072 ads1299_port host self-test
```

This is a software-only test. It does **not** establish STM32F072 board validation, ADS1299 electrical validation, SPI timing margin, EEG performance, EMC/safety, or production readiness.

## Hardware integration invariant

The actual STM32F072 SPI peripheral used by the board layer must be configured for ADS1299 SPI Mode 1 (CPOL=0, CPHA=1 / first edge capture convention as represented by the selected STM32 HAL settings) and MSB-first. Keep network/USB/UART output outside timing-critical DRDY handling; acquisition should remain bounded and non-blocking as the model integration matures.
