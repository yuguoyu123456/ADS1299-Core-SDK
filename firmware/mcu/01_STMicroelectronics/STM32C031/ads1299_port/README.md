# STM32C031 ADS1299 port layer

This directory is the platform-adaptation boundary between the shared ADS1299 core in `firmware/core_driver/ads1299` and an STM32C031 board project. ADS1299 register policy belongs in the shared core; STM32 HAL/LL or user-board details belong outside that core and are connected through `ads1299_platform_hal_t`.

## What this layer provides

- `ads1299_spi.*` — adapts one platform SPI transfer callback to the portable `ads1299_port_t` interface.
- `ads1299_gpio.*` — routes CS, RESET, PWDN and START through logical ADS1299 pin identifiers.
- `ads1299_drdy.*` — presents ADS1299 DRDY using the core convention: the physical signal is active low, so a low GPIO level means data is ready.
- `ads1299_port_selftest.c` — host-only smoke test for the adapter itself. It uses a fake HAL and requires no STM32 board or ADS1299 hardware.
- `Makefile.host` — builds the host smoke test against the real shared `ads1299_port.h` contract.

## Board integration contract

The STM32C031 board layer should create one `ads1299_platform_hal_t` and provide exactly these callbacks:

1. `spi_transfer` — full-duplex transfer using the board-selected STM32 SPI instance. Configure that peripheral for ADS1299 SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Do not put ADS1299 register configuration in this callback.
2. `pin_write` — map the logical IDs `ADS1299_PIN_CS`, `ADS1299_PIN_RESET`, `ADS1299_PIN_PWDN`, and `ADS1299_PIN_START` to the board GPIOs.
3. `pin_read` — return the electrical level of `ADS1299_PIN_DRDY`.
4. `delay_us` — provide a blocking microsecond delay suitable for short device-control delays.

A beginner changing boards should therefore change the board/config implementation that supplies these callbacks, not the files in this directory and not the shared ADS1299 driver.

## Host smoke test

From this directory on a host with a C99 compiler and `make`:

```sh
make -f Makefile.host test
```

Expected result:

```text
PASS STM32C031 ads1299_port host self-test
```

The smoke test checks required callback validation, portable port construction, SPI forwarding, GPIO signal routing, active-low DRDY semantics, delay forwarding and invalid-transfer guards. A passing host test proves only the software adapter behavior; it does **not** prove STM32 pin mapping, SPI electrical timing, ADS1299 communication, signal quality, EMC, safety or hardware readiness.

## If the host test passes but hardware does not

Check the board/config layer before changing the shared ADS1299 core: confirm SPI peripheral selection and Mode 1 settings, MSB-first transfer, CS wiring, DRDY polarity and GPIO input configuration, RESET/START/PWDN wiring, ADS1299 clock, supplies and reference circuitry. Then use the model-level probe/ID example to distinguish a board-port problem from a device/configuration problem.
