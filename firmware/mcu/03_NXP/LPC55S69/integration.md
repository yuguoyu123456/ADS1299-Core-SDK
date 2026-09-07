# Integration with MCUXpresso SDK

1. Install the official `MCUXpresso SDK` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `LPC55S69JBD100` / `LPCXpresso55S69`.
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
callback adapter and ADS1299-independent tests. Pin `MCUX_2.16.000 / 6f3fd257; Core0 reference ELF verified` in the
consumer project and record any API change in `version.md`.

## Concrete MCUX binding

Add mcux_adapter/ and compile the real fsl_spi.c with finite SPI_RETRY_TIMES=100000. Use either board/reference_image.c or your own board binding, not both. The reference owns GPIO ports 0/1 and Flexcomm7 exclusively at cold boot. It must not run as a nonsecure child image; do not alter security fuses. Follow board/reference_image.md before enabling the wiring interlock.
