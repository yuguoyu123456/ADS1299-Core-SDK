# GD32F450Z-EVAL ADS1299 reference source

Status: **Reference source / build-pending**. Hardware validation pending.

Target: GD32F450ZKT6, GD32F450Z-EVAL, GD32F4xx Firmware Library.

This directory uses the shared `firmware/mcu/common/reference_app/` firmware: ADS1299 reset, ID read, 250 SPS, gain 24, internal-test routing, RDATAC/START, DRDY polling, 27-byte frame decoding and common 49-byte packet output. Only the MCU BSP is board-specific.

Required ADS1299 signals: SCK, MISO, MOSI, CS, DRDY, RESET, PWDN, START plus a TX output. The first bring-up uses software SPI Mode 1 and software TX so the BSP only needs GPIO and microsecond timing.

Do not label this target `Compiles` until a clean build against the documented GD32 SDK is recorded.
