# Integration with STM32CubeG4

1. Install the official `STM32CubeG4` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32G474RE` / `NUCLEO-G474RE; provisional control GPIO profile`.
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
callback adapter and ADS1299-independent tests. Pin `CubeG4 1.5.2 linker; CMSIS device 626ee412; GNU reference verified` in the
consumer project and record any API change in `version.md`.

## Concrete CMSIS binding

Add cmsis_adapter/ads1299_g4_hal.c, standard Port and shared Core to your official Cube project. Use either your own board binding or board/reference_image.c, never both. The reference requires HSI16 reset clocks and owns SPI1; do not also initialize it through HAL. See board/reference_image.md.
