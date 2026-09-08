# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32F091` / `Select an official STM32F091 evaluation board`.
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

## Current NUCLEO-F091RC integration path

The earlier callback-only recipe above is retained for compatibility. The current STM32F091 folder now provides the repository-owned HAL binding, so a beginner does **not** need to implement `board_ads1299_hal()` from scratch.

Reference target: **NUCLEO-F091RC / STM32F091RCT6** using STM32CubeIDE and STM32CubeF0.

### CubeMX configuration

Configure these peripherals before generating code:

- SPI1: Master, Full Duplex, 8-bit, MSB first, clock polarity Low, clock phase 2Edge. This is ADS1299 SPI Mode 1.
- Software-controlled CS GPIO.
- Independent RESET, PWDN, START and DRDY GPIOs using the labels required by `board/board_config.h`.
- USART2 when `ADS1299_BOARD_USE_UART_STREAM` is enabled.

Use a conservative SPI clock during initial bring-up. Do not move CS toggling into `HAL_SPI_TransmitReceive`; the ADS1299 port owns CS separately.

### Repository sources to add to the generated project

Add the existing repository modules rather than copying register logic into the STM32 project:

- shared ADS1299 core sources required by the application from `firmware/core_driver/ads1299/`;
- `firmware/common/data_packet/ads1299_packet.c` for canonical packet streaming;
- `STM32F091/ads1299_port/ads1299_spi.c`;
- `STM32F091/ads1299_port/ads1299_gpio.c`;
- `STM32F091/ads1299_port/ads1299_drdy.c`;
- `STM32F091/examples/stm32f091_example_platform.c`;
- `STM32F091/examples/stm32f091_beginner_demo.c`.

Add include paths for the shared core, common packet module, `ads1299_port`, `board`, and `examples` directories.

### Application entry

After `HAL_Init()`, clock initialization and CubeMX-generated GPIO/SPI/UART initialization:

```c
#include "stm32f091_beginner_demo.h"

/* 1000 packets then stop; use 0 for continuous demonstration streaming. */
(void)stm32f091_ads1299_beginner_demo(1000u);
```

The demo performs probe/ID, internal test, input-short validation, 250-SPS EEG configuration, DRDY-driven reads and canonical packet streaming. The older `examples/main_ads1299.c` remains available and its `board_ads1299_hal()` dependency is now supplied by `stm32f091_example_platform.c`.

### Ownership boundary for board changes

For another STM32F091 board, change CubeMX peripheral/pin assignments and the board/config binding. A normal board port must not require edits to shared ADS1299 register/model sources.

### Validation status

This is a concrete integration recipe backed by repository source files, but no STM32CubeIDE compile/link or physical-board execution is claimed here yet. Promote to BUILD-VERIFIED or BOARD-VERIFIED only with corresponding evidence.
