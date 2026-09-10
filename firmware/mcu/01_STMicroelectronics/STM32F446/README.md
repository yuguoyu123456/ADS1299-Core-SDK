# STM32F446 ADS1299 Port

Global ecosystem rank: **106**. Current round status: **integration candidate-complete**. Hardware validation is not implied.

## Reference path

- MCU: **STM32F446RET6**
- Reference board: **NUCLEO-F446RE (MB1136)**
- Toolchain: **STM32CubeMX + STM32CubeIDE + STM32CubeF4 HAL**
- Beginner board configuration: `board/board_config.h`
- Platform adapter: `ads1299_port/`
- Progressive beginner example: `examples/stm32f446_beginner_demo.c`
- Host integration tests: `tests/Makefile.host`
- Shared ADS1299 behavior: `../../../core_driver/ads1299/`

The normal beginner path must not require changes to shared ADS1299 register, model, frame or packet files.

## Quick Start

1. Start from a CubeMX/CubeIDE project for NUCLEO-F446RE.
2. Configure the pins/peripherals to match `board/board_config.h`.
3. Configure SPI1 as ADS1299 SPI Mode 1 (`CPOL=0`, `CPHA=1`), 8-bit, MSB-first with software-controlled CS.
4. Configure DRDY as input and RESET/PWDN/START as outputs.
5. Enable USART2 for the reference ST-LINK VCP starter stream.
6. Add the model port sources, progressive example sources and the shared ADS1299 core sources described in `integration.md`.
7. After generated GPIO/SPI/UART initialization, call:

```c
(void)stm32f446_ads1299_beginner_demo(1000u);
```

Use `0u` only when continuous streaming is desired.

## Expected beginner progression

The progressive demo performs:

`reset -> SDATAC -> probe/ID -> internal test -> input short -> 250-SPS gain-24 EEG -> canonical stream -> STOP/SDATAC`

Expected success messages include:

- `OK ID ADS1299-8` (or ADS1299-6 / ADS1299-4)
- `OK internal-test frames`
- `OK input-short frames`
- `OK EEG 250 SPS configured`
- `OK beginner demo complete`

Diagnostics distinguish reset/power, SPI/ID, DRDY timeout, frame-read and host-transport failures.

## Changing to another STM32F446 board

Change only the CubeMX pin/peripheral routing plus `board/board_config.h`. If generated peripheral handle names differ from `hspi1` / `huart2`, adapt only the model example platform binding. Do not edit `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared ADS1299 behavior for an ordinary board change.

## Performance boundary

The blocking HAL example is for first bring-up at 250 SPS. Sustained higher-rate or multi-ADS1299 acquisition should use interrupt/DMA-driven acquisition with bounded buffering and explicit overflow accounting. UART/printf/network transport must not block the DRDY timing path.

## Validation status

- Board/config layer: present.
- STM32F446 HAL/portable port: present.
- Progressive beginner flow: present.
- Host integration smoke-test recipe: present.
- Host test PASS: **not recorded**.
- STM32CubeIDE clean build for the progressive demo: **not recorded**.
- NUCLEO-F446RE + ADS1299 BOARD-VERIFIED: **not recorded**.
- Sustained acquisition / multi-ADS1299 / 64-channel validation: **not recorded**.

The older `examples/main_ads1299.c` path remains available for compatibility; new students should start with the progressive example above.

The maintenance rank is a repository ordering identifier, not a sales or market-share ranking.
