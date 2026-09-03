# MCU Reference Projects

Choose your controller vendor, then choose the exact board/MCU project.

Each visible project should contain everything needed to understand and build it: entry point, board setup, SPI/GPIO/DRDY integration, local ADS1299 port layer, build files, pin map, bring-up procedure and expected output.

## Current complete-project track

- `st/stm32f407_black/`
- `espressif/esp32s3_devkitc/`
- `raspberry_pi/rp2040_pico/`

More MCU families are added only when they graduate from partial porting work into a reproducible reference project.

## What a complete project must prove

1. Build from a clean environment.
2. Configure SPI Mode 1 and board GPIO correctly.
3. Reset ADS1299 and read the ID register.
4. Read/write registers.
5. Acquire the internal test signal.
6. Read complete 27-byte conversion frames.
7. Decode signed 24-bit channel data correctly.
8. Stream the common packet format to the host.
9. Document exact board, pins, toolchain and limitations.
10. Clearly state whether real hardware and long-run testing have been completed.
