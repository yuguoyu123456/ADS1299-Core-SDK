# Integration with MCUXpresso SDK

1. Install the official `MCUXpresso SDK` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `MIMXRT1176DVMAA` / `MIMXRT1170-EVKB`.
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
callback adapter and ADS1299-independent tests. Pin `25.06.00 reference` in the
consumer project and record any API change in `version.md`.

## EVKB integration checklist

1. Install MCUXpresso SDK 25.06.00 (or record the newer selected version) for
   `evkbmimxrt1170`, then create a Cortex-M7 bare-metal project.
2. Enable LPSPI1, GPIO and the SDK common delay service.
3. Configure LPSPI1 for master, 8-bit, MSB-first, CPOL=0, CPHA=second edge,
   4 MHz. Route SCK/SOUT/SIN to GPIO_AD_28/30/31.
4. Configure Arduino D10 as software GPIO CS, D2 as DRDY input, and D3-D5 as
   RESET/PWDN/START outputs. Generate mux code with the Pins tool.
5. Add Core, `ads1299_port/*.c`, and the MCUXpresso adapter. Populate the GPIO
   base/pin pairs in `ads1299_mcuxpresso_ctx_t`.
6. Keep USB/Ethernet/UART packet transport in `firmware/transport`.
