# GD32F450Z-EVAL ADS1299 reference firmware

Status: **Compiles** — clean GitHub Actions build verified. Hardware validation pending.

Target: **GD32F450ZKT6** on **GD32F450Z-EVAL**. The build uses the GD32F4xx SPL through a pinned PlatformIO package; GigaDevice's current GD32F4xx Firmware Library is the normative vendor SDK family for this target.

The firmware reuses `firmware/mcu/common/reference_app/`: ADS1299 reset, ID read, 250 SPS, gain 24, internal-test routing, RDATAC/START, DRDY polling, 27-byte frame decoding and common 49-byte packet output. The GD32-specific code is only the GPIO/timing BSP in `src/board_sdk.c`.

## Default bring-up pin map

| ADS1299 / stream | GD32F450 pin |
|---|---|
| SCK | PA5 |
| MISO | PA6 |
| MOSI | PA7 |
| CS | PB0 |
| DRDY | PB1 |
| RESET | PB2 |
| PWDN | PB10 |
| START | PB11 |
| binary TX | PA9 |

The first bring-up intentionally uses software SPI Mode 1 and software 500 kbaud TX. This minimizes vendor-peripheral coupling while validating the portable ADS1299 core; hardware SPI/DMA can replace the BSP later without changing the device API.

## Clean build

```bash
python -m pip install platformio==6.1.18
pio run -d firmware/mcu/gigadevice/gd32f450z_eval -e gd32f450z_eval
```

`platformio.ini` pins both the GD32 PlatformIO integration and GD32 SPL package commits so CI does not silently move with upstream HEAD. The dedicated `gd32-reference` workflow has completed this command successfully from a fresh Ubuntu runner.

`Compiles` is a software-build claim only. No Bench-tested or 24h-tested claim is made without the real GD32F450 board and ADS1299 hardware.
