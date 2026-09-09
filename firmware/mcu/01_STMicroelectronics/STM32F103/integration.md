# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32F103` / `Select an official STM32F103 evaluation board`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c` and include path.
5. Implement `board_ads1299_hal()` using the vendor SDK's SPI, GPIO and microsecond-delay APIs.
6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.
7. Copy the call flow from `examples/main_ads1299.c`; keep product transport outside the example.
8. First verify power, ID read, register readback and internal test input. Only then connect electrodes.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware
stay in the user's official SDK project. This repository owns only the thin
callback adapter and ADS1299-independent tests. Pin `pin in the consuming official SDK project` in the
consumer project and record any API change in `version.md`.

---

## Current STM32F103 reference integration

The original generic callback procedure above is retained for compatibility. For the current reference path, users no longer need to design `board_ads1299_hal()` from scratch.

### Reference board and generated project

Use **NUCLEO-F103RB (MB1136, STM32F103RBT6)** as the first reproducible reference configuration.

Generate the project with STM32CubeMX/STM32CubeIDE and keep vendor-owned startup, linker, CMSIS and HAL files in that generated project. Configure:

- SPI1 master, full duplex, 8-bit, MSB-first, CPOL Low, CPHA 2 Edge, software NSS;
- GPIO labels `ADS1299_CS`, `ADS1299_RESET`, `ADS1299_PWDN`, `ADS1299_START`, `ADS1299_DRDY` as documented in `board/README.md`;
- USART2 TX only when the starter UART streaming path is desired.

### Repository sources to add

At minimum, add the shared ADS1299 core sources required by the existing core build plus:

- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32f103_example_platform.c`
- `examples/stm32f103_beginner_demo.c`

Also add include paths for the shared core, `ads1299_port/`, `board/` and `examples/`.

`examples/stm32f103_example_platform.c` binds the portable `ads1299_port_t` callbacks to STM32F1 HAL using the hardware choices centralized in `board/board_config.h`. The older `examples/main_ads1299.c` path remains available for compatibility.

### Beginner execution path

Call:

`stm32f103_ads1299_beginner_demo(frame_limit)`

Use a finite frame limit during first bring-up; use the example's documented continuous option only after ID/self-test/input-short checks pass. The demo is intended to expose distinct diagnostics for configuration/port failure, SPI/ID failure, DRDY timeout, frame failure and transport failure.

### Board changes

For another board using the same MCU family, normal customization should stay in CubeMX plus `board/board_config.h` (SPI/UART handles and generated GPIO routing). Do not edit shared ADS1299 register/model files merely to move pins or peripherals.

### Validation boundary

This integration is **TEMPLATE / integration candidate-complete**. The repository contains the binding and runnable flow, but no clean STM32CubeIDE target build is claimed here yet, and no physical NUCLEO-F103RB + ADS1299 run is claimed. Promote those states only with recorded build or board evidence.
