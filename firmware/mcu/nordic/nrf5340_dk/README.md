# nRF5340 DK ADS1299 reference firmware

Status: **Compiles** with clean GitHub Actions build against **nRF Connect SDK v3.4.0 LTS / Zephyr**. Hardware validation pending.

Target: **nRF5340 application core** on **nRF5340 DK**, using board target `nrf5340dk/nrf5340/cpuapp`.

The firmware reuses `firmware/mcu/common/reference_app/`: ADS1299 reset, ID read, 250 SPS, gain 24, internal-test routing, RDATAC/START, DRDY polling, 27-byte frame decoding and common 49-byte packet output. The nRF5340-specific code is only the GPIO/timing BSP in `src/board_sdk.c`.

## Default bring-up pin map

Zephyr's nRF5340 DK DTS maps Arduino D2..D10 to P1.04..P1.12. This profile uses those pins so it does not consume the board LEDs/buttons on P0.

| ADS1299 / stream | nRF5340 DK |
|---|---|
| SCK | D2 / P1.04 |
| MISO | D3 / P1.05 |
| MOSI | D4 / P1.06 |
| CS | D5 / P1.07 |
| DRDY | D6 / P1.08 |
| RESET | D7 / P1.09 |
| PWDN | D8 / P1.10 |
| START | D9 / P1.11 |
| binary TX | D10 / P1.12 |

The first bring-up intentionally uses software SPI Mode 1 and software packet TX through the shared BSP contract. A later performance pass can replace these with SPIM/GPIOTE/DPPI/UARTE without changing the ADS1299 core API.

## Clean build

Inside an nRF Connect SDK v3.4.0 workspace:

```bash
west build -p always --no-sysbuild -b nrf5340dk/nrf5340/cpuapp firmware/mcu/nordic/nrf5340_dk
```

GitHub Actions uses Nordic's `ghcr.io/nrfconnect/sdk-nrf-toolchain:v3.4.0` container and initializes a pinned NCS v3.4.0 workspace before building. The clean build succeeded before this status was upgraded.

This is a **compile-level** claim only. No Bench-tested or 24h-tested claim is made without real hardware evidence.
