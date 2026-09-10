# STM32G071 ADS1299 port

This directory adapts STM32G071 board/HAL services to the shared portable `ads1299_port_t` contract. ADS1299 register policy, profiles, frame parsing, packet formatting and multi-device behavior belong in `firmware/core_driver/ads1299/`, not in this folder.

## Files

- `ads1299_spi.c/.h` — SPI callback adapter and `ads1299_mcu_port_init()` / `ads1299_mcu_make_port()` entry points.
- `ads1299_gpio.c/.h` — CS, RESET, PWDN, START and delay routing.
- `ads1299_drdy.c/.h` — DRDY read helpers. ADS1299 DRDY is active low.
- `ads1299_port_selftest.c` — host-only fake-HAL smoke test for the adapter contract.
- `Makefile.host` — strict C99 host build for the adapter smoke test.

## Host smoke test

From this directory run:

```sh
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32G071 ads1299_port host self-test
```

The test checks SPI callback forwarding and invalid-argument handling, CS/RESET/PWDN/START GPIO routing, DRDY active-low semantics and delay forwarding. It does not emulate an ADS1299 register map and does not prove hardware operation.

## Board integration rule

A new STM32G071 board should provide concrete STM32 HAL callbacks and pin/peripheral selection in the model's board/config layer, then populate `ads1299_platform_hal_t`. Do not edit shared ADS1299 core files for normal board bring-up.

The board SPI peripheral must be configured for ADS1299 requirements: SPI Mode 1 (`CPOL=0`, `CPHA=1`), 8-bit transfers and MSB-first. Keep CS under software control so a shared SPI bus can use independent chip selects for multiple ADS1299 devices.

STM32G071 is a resource-conscious MCU, so keep acquisition memory static and bounded. Avoid heap allocation and floating-point work in the DRDY timing path. For sustained acquisition, keep ISR work short, place raw frames into a fixed-size queue/ring buffer, process/stream them outside the ISR, and count queue overflow explicitly.

## Validation status

- Adapter source presence: PRESENT.
- Host self-test source/build recipe: PRESENT.
- Host self-test execution: NOT RECORDED by this documentation.
- STM32 toolchain build: not implied by the host smoke test.
- Physical STM32G071 + ADS1299 board validation: NOT CLAIMED.
