# nRF52840 DK ADS1299 reference firmware

Status: **Reference source / build-pending** until the clean NCS build on this branch succeeds. Hardware validation pending.

Target: **Nordic nRF52840 DK / nRF52840** using **nRF Connect SDK v3.4.0 LTS / Zephyr**. The build target is `nrf52840dk/nrf52840`, matching Nordic's current NCS build documentation.

The application reuses `firmware/mcu/common/reference_app/`, so ADS1299 reset, ID read, 250 SPS, gain 24, internal-test routing, RDATAC/START, DRDY polling, 27-byte frame decode and common 49-byte packet output remain one shared implementation. `src/board_sdk.c` is the only Nordic-specific execution layer.

## Default bring-up pin map

The nRF52840 DK Zephyr board definition exposes Arduino D2..D10 on GPIO1. This profile uses those nine header pins so it does not consume the DK's P0.13..P0.16 LEDs or P0.11/P0.12/P0.24/P0.25 buttons.

| ADS1299 / stream | Arduino header | nRF52840 GPIO |
|---|---|---|
| SCK | D2 | P1.03 |
| MISO | D3 | P1.04 |
| MOSI | D4 | P1.05 |
| CS | D5 | P1.06 |
| DRDY | D6 | P1.07 |
| RESET | D7 | P1.08 |
| PWDN | D8 | P1.10 |
| START | D9 | P1.11 |
| binary TX | D10 | P1.12 |

The first bring-up intentionally uses software SPI Mode 1 and software 500 kbaud TX through Zephyr GPIO plus `k_busy_wait()`. Hardware SPIM/UARTE/EasyDMA is a later performance optimization and will not change the ADS1299 API.

## Clean build

With an nRF Connect SDK v3.4.0 workspace active:

```bash
west build -p always --no-sysbuild -b nrf52840dk/nrf52840 firmware/mcu/nordic/nrf52840_dk
```

The dedicated `nrf52-reference` workflow initializes NCS v3.4.0 and runs this command in Nordic's SDK toolchain container. Do not label the target `Compiles` until that clean build succeeds. No Bench-tested claim is made without real nRF52840 DK + ADS1299 hardware.
