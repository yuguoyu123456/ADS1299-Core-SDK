# Raspberry Pi Pico 2 / RP2350 — ADS1299 native reference project

Status: **Compiles — clean GitHub Actions build verified with Pico SDK 2.3.0. Hardware validation pending.**

This project is the RP2350/Pico 2 counterpart to `rp2040_pico`. It reuses the same ADS1299 Pico port and application structure while compiling for `PICO_BOARD=pico2` under Pico SDK 2.3.0.

First profile: SPI Mode 1, 250 SPS, gain 24, internal test source, DRDY-driven 27-byte frame acquisition and repository common packet output. Pin definitions remain the same as the RP2040 reference until dedicated Pico 2 hardware validation records otherwise.

Build from repository root after setting `PICO_SDK_PATH`:

```bash
cmake -S firmware/mcu/raspberry_pi/rp2350_pico2 -B build/rp2350 -DPICO_BOARD=pico2 -G Ninja
cmake --build build/rp2350
```

GitHub Actions has verified clean configure and build with Pico SDK 2.3.0. This is a **Compiles** claim only; no Bench-tested or 24h-tested claim is made.
