# Integration with STM32CubeF4

1. Install the official `STM32CubeF4` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32F407VGT6` / `STM32F407G-DISC1 (MB997)`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c` and include path.
5. For the maintained beginner path, use `board/board_config.h` plus `examples/stm32f407_example_platform.c`; the compatibility `board_ads1299_hal()` wrapper is already supplied there for the older example.
6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.
7. Add `examples/stm32f407_beginner_demo.c` and call `stm32f407_ads1299_beginner_demo(1000u)` after Cube-generated peripheral initialization.
8. First verify power, ID read, internal test and input-short. Then run 250-SPS EEG and canonical streaming. Only then connect electrodes to a properly designed analog front end.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in the platform binding. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware
stay in the user's official SDK project. This repository owns the board/config
contract, thin HAL/port adapter, ADS1299 examples and host/integration tests. Pin
`STM32CubeF4 1.28.3` in the consumer project when reproducing the recorded build
and record any API change in `version.md`.

`stm32cube_adapter/` binds the official HAL and
`examples/stm32cube_reference/main.c` supplies the minimum MB997 setup. The
PlatformIO build is documented in `build.md`. Host UART/USB/network output must
be attached outside the shared ADS1299 Core, and transport must not block the
DRDY timing path.

The reference uses polling for initial bring-up. A DMA implementation must place
DMA buffers outside CCM, use bounded queues, expose overflow counters and record
stream-loss measurements before any sustained-acquisition or multi-device
hardware status upgrade.

## Beginner ownership rule

For another STM32F407 board, the normal hardware-dependent changes are limited to
CubeMX pin/peripheral routing and `board/board_config.h` (plus generated handle
names consumed by the example platform binding). Do not edit shared ADS1299
register, model, frame or packet logic merely to move between STM32F407 boards.

## Validation boundary

A clean PlatformIO/STM32CubeF4 build was recorded on 2026-09-04 and is valid
software BUILD-VERIFIED evidence for the documented reference configuration.
That record does not prove ADS1299 wiring, physical-board operation, sustained
streaming or multi-ADS1299 behavior. The newer progressive beginner demo has not
yet been separately recorded as a clean STM32CubeIDE build.
