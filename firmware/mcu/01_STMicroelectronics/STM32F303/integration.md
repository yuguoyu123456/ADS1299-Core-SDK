# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32F303` / `Select an official STM32F303 evaluation board`.
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

## Current NUCLEO-F303RE integration path

The generic callback recipe above is preserved for compatibility. New users should prefer the repository-provided STM32F303 binding instead of implementing `board_ads1299_hal()` from scratch.

1. Create a **NUCLEO-F303RE** project in STM32CubeMX/STM32CubeIDE.
2. Configure SPI1 and ADS1299 control GPIOs exactly as documented in `board/README.md`; SPI must be master, full-duplex, 8-bit, MSB-first, CPOL Low, CPHA 2Edge (Mode 1).
3. Keep generated HAL/startup/linker files in the vendor project. Do not copy or invent them in this SDK.
4. Add the repository shared ADS1299 core sources required by the existing project build, plus this model's `ads1299_port/*.c`.
5. Add `examples/stm32f303_example_platform.c` and `examples/stm32f303_beginner_demo.c`.
6. Include `board/board_config.h`; this is the obvious repository-owned place for SPI/UART handle selection and board-level choices.
7. Call `stm32f303_ads1299_beginner_demo(frame_count)` from the generated application after HAL/peripheral initialization. A frame count of zero selects continuous streaming in the current example contract.
8. Observe the example diagnostics in order: platform/configuration, reset/command path, ID probe, internal test, input-short, DRDY/frame read, packet transport, stop.

For another STM32F303 board, change CubeMX pin/peripheral routing and the board/config handles. Do not modify shared ADS1299 register/model files for normal board migration.

### Validation boundary

This integration recipe is repository-present, but no clean STM32CubeIDE build or physical-board run is claimed here until an actual recorded validation exists.