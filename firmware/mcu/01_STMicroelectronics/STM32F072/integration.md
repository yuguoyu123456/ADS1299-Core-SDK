# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32F072` / `Select an official STM32F072 evaluation board`.
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

## Current STM32F072 integration path

The original generic steps above are retained for history. The current model folder now provides the HAL binding that the old step 5 asked the user to write manually.

### Reference target

- board: **NUCLEO-F072RB (MB1136)**
- MCU: **STM32F072RBT6**
- generated project owner: STM32CubeIDE / STM32CubeMX
- repository board configuration: `board/board_config.h`
- repository HAL binding: `examples/stm32f072_example_platform.c/.h`
- beginner flow: `examples/stm32f072_beginner_demo.c/.h`

### CubeMX configuration

1. Create a project for NUCLEO-F072RB.
2. Enable SPI1 as master/full-duplex, 8-bit, MSB-first, CPOL Low, CPHA 2Edge.
3. Keep NSS/CS under software GPIO control.
4. Create GPIO labels `ADS1299_CS`, `ADS1299_RESET`, `ADS1299_PWDN`, `ADS1299_START`, `ADS1299_DRDY` using the mapping in `board/README.md`.
5. Configure DRDY as an input for the initial polling path. An EXTI/DMA design may be added later, but the beginner example does not depend on it.
6. Enable USART2 if using the reference UART log/stream transport.
7. Generate the vendor project normally. Do not replace the generated startup, clock or linker configuration with repository guesses.

### Repository-owned files to add

Add these sources to the generated project:

- shared core sources from `firmware/core_driver/ads1299/`
- `firmware/common/data_packet/ads1299_packet.c`
- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32f072_example_platform.c`
- `examples/stm32f072_beginner_demo.c`

Add include paths for the shared core, common packet module, `ads1299_port`, `board` and `examples`.

The old `examples/main_ads1299.c` remains available for compatibility; its `board_ads1299_hal()` dependency is now supplied by `stm32f072_example_platform.c`, so a new student should not create another competing HAL layer.

### Application entry

After generated initialization has completed, call:

```c
#include "stm32f072_beginner_demo.h"

(void)stm32f072_ads1299_beginner_demo(1000u);
```

Use `0u` only when an intentionally continuous stream is desired. The example uses a blocking transport for clarity; production acquisition should isolate transport work from DRDY timing with interrupt/DMA and bounded buffering.

### Expected progression

The example performs ID/probe, internal-test acquisition, input-short acquisition, 250-SPS EEG setup and packet streaming before stopping cleanly for a finite packet count. Detailed expected log text and failure meanings are in `examples/README.md`.

### Validation boundary

This integration recipe is **TEMPLATE / integration candidate-complete**. The repository does not currently claim a complete STM32CubeIDE build artifact or a physical NUCLEO-F072RB + ADS1299 run. A future BUILD-VERIFIED label requires an actual target compile/link using the documented generated project; BOARD-VERIFIED additionally requires evidence from real hardware.
