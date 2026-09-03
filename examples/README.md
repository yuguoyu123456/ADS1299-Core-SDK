# Reference projects

`ports/` contains thin adapters. `examples/` contains complete or near-complete reference applications tied to a specific vendor toolchain/board.

Every example must provide:

- exact target MCU/board
- exact SDK/compiler/tool version
- pin table
- SPI mode/clock
- reset/start/DRDY behavior
- build instructions
- expected serial output
- validation label

The first complete example will be STM32 HAL. GD32, CH32 and ESP32 follow after the common driver API is stable.
