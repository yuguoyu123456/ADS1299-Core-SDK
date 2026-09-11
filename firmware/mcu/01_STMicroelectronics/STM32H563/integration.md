# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32H563` / `Select an official STM32H563 evaluation board`.
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

## Current STM32H563 reference integration

The preferred reference path is now **NUCLEO-H563ZI / STM32H563ZIT6** using STM32CubeH5. The generic callback instructions above remain valid for legacy/custom boards, but new users should start with the repository-owned board configuration and progressive example.

### Files to add to the Cube project

Model-local sources:

- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32h563_example_platform.c`
- `examples/stm32h563_beginner_demo.c`

Model-local include paths:

- `board/`
- `ads1299_port/`
- `examples/`

Add the shared ADS1299 sources required by the current core API, including the driver, frame/model helpers and packet encoder already maintained under `firmware/core_driver/ads1299/`. Do not copy those sources into the STM32H563 folder.

### One obvious board edit point

Edit `board/board_config.h` for hardware-dependent pins, SPI/UART instances, timeouts and bounded buffering choices. Keep ADS1299 register definitions and profile logic in the shared core.

The reference mapping uses SPI1 with software CS plus independent DRDY, RESET, PWDN and START GPIOs, and USART3 as the starter host stream. Configure SPI as **CPOL=0, CPHA=1, MSB first**.

### Bring-up sequence

1. Generate/build the STM32CubeH5 project with the board GPIO/SPI/UART configuration matching `board/board_config.h`.
2. Call `stm32h563_ads1299_beginner_demo(1000u)` after Cube peripheral initialization.
3. Confirm the log reaches ADS1299 probe/ID before advancing to self-test profiles.
4. Run internal-test, then input-short validation.
5. Run the 250-SPS gain-24 EEG profile.
6. Confirm canonical packet streaming and clean STOP/SDATAC shutdown.

### Sustained acquisition architecture

Blocking HAL calls are acceptable for first probe/self-test bring-up. For sustained EEG, use a short DRDY falling-edge ISR, capture timing/state, hand work to a bounded static queue/ring buffer, and perform packetization/host transport outside the interrupt path. Record queue overflow explicitly. If SPI DMA is enabled, keep CS/frame boundaries deterministic and do not reuse a transfer buffer before DMA completion; apply cache maintenance only when required by the actual memory/cache configuration.

### Validation boundary

These instructions are an integration recipe, not evidence of a successful target compile or board run. Record BUILD-VERIFIED and BOARD-VERIFIED only after those steps have actually succeeded and been captured in `validation.md`.
