# STM32G071 ADS1299 Port

Global ecosystem rank: **115**. Current status: **Integration candidate-complete**. Tier C. Hardware validation is not implied.

## Reference platform

- Vendor: STMicroelectronics
- MCU: STM32G071RBT6
- Reference board: **NUCLEO-G071RB (MB1360)**
- Official environment: STM32CubeG0 / STM32CubeIDE
- Debug/host link: on-board ST-LINK, USART2 VCP

## Beginner path

1. Open `board/README.md` and wire the ADS1299 to the NUCLEO-G071RB reference mapping.
2. Edit only `board/board_config.h` for repository-owned board choices. If CubeMX generated different peripheral handle names, adjust only the model-local HAL binding in `examples/stm32g071_example_platform.c`.
3. Configure SPI1 as Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, software CS.
4. Add the model port/example sources, the shared ADS1299 core, and `firmware/common/data_packet/ads1299_packet.c` to the Cube project as listed in `integration.md`.
5. Call `stm32g071_ads1299_beginner_demo(1000u)` after Cube/HAL initialization. Use `0u` only for intentional continuous streaming.
6. Observe probe/ID, internal-test, input-short, 250-SPS EEG configuration, canonical packet streaming, and clean stop.

A normal first bring-up must **not** require editing `firmware/core_driver/ads1299/ads1299.c`, `ads1299_regs.h`, `ads1299_model.c`, or other shared-core implementation files.

## Repository layers

- Shared ADS1299 behavior: `../../../core_driver/ads1299/`
- Shared canonical packet encoder: `../../../common/data_packet/`
- Hardware-only STM32G071 adapter: `ads1299_port/`
- Single board/config entry: `board/board_config.h`
- Recommended progressive demo: `examples/stm32g071_beginner_demo.c`
- HAL binding: `examples/stm32g071_example_platform.c`
- Compatibility/minimal example: `examples/main_ads1299.c`
- Host/integration tests: `tests/`
- Vendor project procedure: `integration.md`

## Reference behavior

The starter configuration uses 250 SPS, gain 24 and the repository canonical 49-byte packet. At 250 SPS, 49-byte packets require 12,250 payload bytes/s, so the reference VCP stream uses 460800 baud rather than 115200 baud.

STM32G071 is treated as a small-MCU target: keep DRDY-path work short, use static bounded storage, avoid heap allocation and floating-point conversion in the timing-critical path, and explicitly count queue overflow when moving beyond the blocking bring-up example.

## Current validation status

Repository integration is present for board/config, portable port, progressive examples and host-test recipes. This is **not** a BUILD-VERIFIED or BOARD-VERIFIED claim. No sustained-acquisition, multi-ADS1299, 64-channel, 24-hour, electrical-safety, EMC or production-readiness claim is made.

The older planning text for this maintenance slot has now been superseded by the concrete NUCLEO-G071RB integration above; rank 115 remains only a repository maintenance identifier, not a sales ranking.

[Official STM32G0 family entry](https://www.st.com/en/microcontrollers-microprocessors/stm32g0-series.html) · [101–200 maintenance list](../../ECOSYSTEM_101_200.md)
