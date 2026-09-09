# STM32F446 ADS1299 examples

Status: **TEMPLATE / repository integration present**. The code below is not claimed BOARD-VERIFIED.

Reference path: **NUCLEO-F446RE (MB1136, STM32F446RET6)** with STM32CubeMX/STM32CubeIDE and STM32CubeF4 HAL.

## Beginner path

Use `../board/board_config.h` as the single obvious repository-owned hardware configuration point. Configure CubeMX to match that file:

- SPI1, Mode 1 (`CPOL=0`, `CPHA=1`), 8-bit, MSB-first;
- software-controlled CS;
- DRDY as input;
- RESET/PWDN/START as outputs;
- USART2 for the starter host stream over the default NUCLEO-F446RE ST-LINK VCP route.

The generated handles used by this example are `hspi1` and `huart2`.

Add these model files to the CubeIDE project:

- `../ads1299_port/ads1299_spi.c`
- `../ads1299_port/ads1299_gpio.c`
- `../ads1299_port/ads1299_drdy.c`
- `stm32f446_example_platform.c`
- `stm32f446_beginner_demo.c`

Also include the repository shared ADS1299 core sources/headers required by `ads1299.h`, `ads1299_packet.h`, frame/model and portable-port APIs. Do not copy ADS1299 register logic into this folder and do not edit shared core files for normal board bring-up.

After CubeMX-generated initialization has configured GPIO, SPI1 and USART2, include `stm32f446_beginner_demo.h` and call:

```c
(void)stm32f446_ads1299_beginner_demo(1000u);
```

Use `0u` for continuous streaming. A finite value such as `1000u` is safer for first bring-up.

## Expected progression

The demo performs, in order:

1. hardware reset and `SDATAC`;
2. ADS1299-family ID probe;
3. internal-test configuration and frame capture;
4. input-short configuration and frame capture;
5. 250-SPS, gain-24 EEG channel configuration;
6. continuous acquisition and canonical packet streaming;
7. clean `STOP` + `SDATAC`.

Useful text messages include:

- `OK ID ADS1299-8` (or ADS1299-6 / ADS1299-4);
- `OK internal-test frames`;
- `OK input-short frames`;
- `OK EEG 250 SPS configured`;
- `OK beginner demo complete`.

Errors distinguish RESET/power, SPI/ID, DRDY timeout, frame-read and USART2 transport failures.

## Moving to another STM32F446 board

Change the CubeMX pin/peripheral routing and `../board/board_config.h`. If the generated SPI/UART handles are not `hspi1` / `huart2`, adapt only `stm32f446_example_platform.c`. Shared ADS1299 register/model/frame/packet logic should remain untouched.

## Performance note

The blocking HAL implementation is intentionally a first-bring-up path at 250 SPS. For sustained higher-rate or multi-ADS1299 acquisition, move acquisition and transport to interrupt/DMA-driven code with a bounded queue and explicit overflow counters. Do not allow UART, printf or other host transport work to block the DRDY timing path.

The reference `board_config.h` reserves a bounded starter queue depth for this reason; the example itself remains deliberately simple and polling-based so first-time users can understand failure modes.

## Validation

- Repository integration: present.
- Host compile of this Cube HAL example: not recorded.
- STM32CubeIDE clean build for this exact beginner demo: not recorded.
- NUCLEO-F446RE + ADS1299 hardware run: not recorded.
- Sustained acquisition / multi-ADS1299 validation: not recorded.

The older `main_ads1299.c` is retained unchanged for compatibility.
