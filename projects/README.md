# Complete reference projects

This directory replaces the old idea that a small adapter file is enough to call a platform supported.

Each subdirectory is intended to be a real buildable firmware project with a board target, pin map, `main`, ADS1299 bring-up, DRDY streaming, common packet output and build instructions.

Current gold-reference work starts with STM32F407, then ESP32-S3, RP2040/RP2350, GD32F407, CH32V307 and nRF52840. Other MCU-family folders remain `Scaffold` until they meet `docs/PROJECT_QUALITY_STANDARD.md`.
