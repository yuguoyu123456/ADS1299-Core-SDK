# STM8 ADS1299 ready-to-run path

This directory remains supported as a small-controller integration target, but STM8 is different from STM32Cube HAL targets. Reuse this folder's existing STM8-specific port/board code and the portable ADS1299 core only if the selected STM8 device has sufficient flash/RAM for the required feature set.

Bring-up should stay minimal: SPI mode 1 -> GPIO control -> DRDY interrupt/polling -> ID read -> internal test -> input short -> normal input. Read exactly one 27-byte frame per ADS1299-8 conversion.

For resource-constrained STM8 builds, keep only the driver functions actually needed, use raw 24-bit samples, avoid floating-point conversion on-device, and stream/process data on a more capable host.

A full 8 x ADS1299 / 64-channel controller is not the intended STM8 use case; use STM32/FPGA/Linux layers for that architecture.

The STM32 shared helper in `../_shared` documents the application sequencing, but STM8 projects should use the same sequence with their own existing STM8 port rather than including STM32 HAL-specific code.
