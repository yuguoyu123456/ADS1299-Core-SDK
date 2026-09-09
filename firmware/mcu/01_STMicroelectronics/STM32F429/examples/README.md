# STM32F429 ADS1299 examples

Status: **TEMPLATE / repository integration present**. The code below is not claimed BOARD-VERIFIED.

Reference path: **STM32F429I-DISC1 (MB1075, STM32F429ZIT6)** with STM32CubeMX/STM32CubeIDE and STM32CubeF4 HAL.

## Beginner path

Use `../board/board_config.h` as the only repository-owned hardware configuration point. Configure CubeMX to match that file: SPI1 in Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first; software CS; DRDY input; RESET/PWDN/START outputs; USART1 for the starter stream. The generated handles used by this example are `hspi1` and `huart1`.

Add these model files to the CubeIDE project:

- `../ads1299_port/ads1299_spi.c`
- `../ads1299_port/ads1299_gpio.c`
- `../ads1299_port/ads1299_drdy.c`
- `stm32f429_example_platform.c`
- `stm32f429_beginner_demo.c`

Also include the repository shared ADS1299 core sources and headers required by `ads1299.h`, `ads1299_packet.h`, frame/model and port APIs. Do not copy register logic into this folder and do not edit shared core files for normal board bring-up.

After CubeMX initializes GPIO, SPI1 and USART1, call:

```c
(void)stm32f429_ads1299_beginner_demo(1000u);
```

Use `0u` for continuous streaming. A finite value is safer for first bring-up.

## Expected progression

The demo performs, in order:

1. hardware reset and SDATAC;
2. ADS1299-family ID probe;
3. internal-test configuration and frame capture;
4. input-short configuration and frame capture;
5. 250-SPS, gain-24 EEG channel configuration;
6. continuous acquisition and canonical packet streaming;
7. clean STOP + SDATAC.

Useful text messages include `OK ID ADS1299-8`, `OK internal-test frames`, `OK input-short frames`, `OK EEG 250 SPS configured`, and `OK beginner demo complete`. Error messages distinguish RESET/power, SPI/ID, DRDY timeout, frame-read and USART transport failures.

## Moving to another STM32F429 board

Change the CubeMX pin/peripheral routing and `../board/board_config.h`. If the generated SPI/UART handle names are not `hspi1`/`huart1`, adapt only `stm32f429_example_platform.c`. The ADS1299 shared driver/model/register logic should remain untouched.

## Performance note

The blocking HAL implementation is intended for initial bring-up at 250 SPS. For sustained higher-rate or multi-ADS1299 acquisition, use interrupt/DMA acquisition plus a bounded queue and explicit overflow accounting; do not let UART/host transport block the DRDY timing path.

## Validation

- Repository integration: present.
- Host compile of this Cube HAL example: not recorded.
- STM32CubeIDE clean build for this exact beginner demo: not recorded.
- STM32F429I-DISC1 + ADS1299 hardware run: not recorded.
- Sustained acquisition / multi-ADS1299 validation: not recorded.

The older `main_ads1299.c` is retained for compatibility.
