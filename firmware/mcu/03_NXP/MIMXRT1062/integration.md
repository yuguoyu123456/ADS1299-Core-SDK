# Integration with MCUXpresso SDK

1. Install the official `MCUXpresso SDK` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `MIMXRT1062DVL6B` / `MIMXRT1060-EVKB`.
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

1. Create an `evkbmimxrt1060` bare-metal project with MCUXpresso SDK 25.06.00
   or record the newer SDK version actually used.
2. Configure LPSPI1 master: 8-bit, MSB-first, CPOL=0, CPHA=second edge, 4 MHz.
3. Route SCK/SDO/SDI to GPIO_SD_B0_00/02/03; configure D10
   GPIO_SD_B0_01 as software CS.
4. Configure J16 D2 as DRDY input and D3-D5 as RESET/PWDN/START outputs.
5. Add Core, `ads1299_port/*.c`, and the MCUXpresso adapter. Populate all GPIO
   base/pin pairs after MCUXpresso Config Tools generates the mux definitions.
6. Keep OpenSDA VCOM, USB and Ethernet policy in `firmware/transport`.
