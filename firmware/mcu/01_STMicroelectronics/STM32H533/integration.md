# Integration with STM32Cube / official device package

1. Install the official `STM32Cube / official device package` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32H533` / `Select an official STM32H533 evaluation board`.
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

## Current NUCLEO-H533RE integration recipe

The generic recipe above is retained for compatibility. The current reference path is **NUCLEO-H533RE / STM32H533RET6** using STM32CubeH5.

### Cube configuration

Use the concrete mapping in `board/board_config.h` and `board/README.md`. Configure SPI1 for 8-bit full-duplex master operation, CPOL=0, CPHA=1, MSB first, software NSS/CS, and a conservative ADS1299-safe clock for first bring-up. Configure DRDY as an input for polling first; later it may be changed to falling-edge EXTI for sustained acquisition. Configure the documented RESET, PWDN and START pins as push-pull outputs and USART2 as the starter host transport.

Do not add vendor startup files, linker scripts, generated `main.c`, or HAL sources to this repository. Generate and own those files inside the consuming STM32CubeH5 project.

### Repository sources to add

Model-local sources:

- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32h533_example_platform.c`
- `examples/stm32h533_beginner_demo.c`

Shared ADS1299 sources required by the chosen application should come from `../../../core_driver/ads1299/`; retain the existing shared build union rather than copying register logic into this folder. Add the repository's canonical packet implementation when using the streaming example.

Include paths must cover this model's `board/`, `ads1299_port/`, `examples/`, and the shared ADS1299 core/packet headers used by the build.

### Application entry

After STM32Cube-generated clock/GPIO/SPI/UART initialization succeeds, call:

```c
(void)stm32h533_ads1299_beginner_demo(1000u);
```

A zero frame count requests continuous streaming. The finite run is recommended first because it gives a deterministic stop path while wiring and transport are still being checked.

### Expected diagnostic progression

A correctly integrated application should clearly distinguish at least these stages: platform/configuration failure, SPI/ID/probe failure, internal-test failure, input-short failure, DRDY timeout, frame/packet failure, and clean stop. Exact numeric ADS1299 ID values and analog measurements must be judged by the shared driver and the actual device variant rather than duplicated as magic literals here.

### Sustained acquisition

Blocking HAL is acceptable for initial ID/self-test bring-up. For sustained 250-SPS EEG, use a short DRDY interrupt path that timestamps/enqueues work into a static bounded queue or ring buffer. Perform packetization and USART/USB/network transport outside the timing-critical handler and count queue overflow. If SPI DMA is introduced, preserve deterministic CS and frame boundaries and handle cache/DMA memory requirements according to the concrete STM32H533 project configuration.
