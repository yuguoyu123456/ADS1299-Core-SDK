# Integration with STM32CubeMX / STM32CubeIDE

Reference path: **NUCLEO-F446RE (MB1136, STM32F446RET6)** using STM32CubeF4 HAL.

## Files owned by this repository

Use these model-local files:

- `board/board_config.h`
- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32f446_example_platform.c`
- `examples/stm32f446_beginner_demo.c`

Use the shared ADS1299 core under `../../../core_driver/ads1299/` for ADS1299 commands, registers, models, frames, packet/profile behavior and multi-device logic. Do not copy that logic into this MCU folder.

Vendor startup code, linker scripts, CMSIS/HAL and CubeMX-generated initialization remain in the user's official STM32 project.

## Reference Cube configuration

Configure the project to match `board/board_config.h`:

- SPI1: Mode 1 (`CPOL=0`, `CPHA=1`), 8-bit, MSB-first;
- software-controlled CS;
- DRDY input;
- RESET/PWDN/START outputs;
- USART2 as the reference starter transport over the NUCLEO-F446RE ST-LINK VCP route.

The progressive reference binding expects generated handles `hspi1` and `huart2`. If another STM32F446 board uses different handles or peripherals, adapt `board/board_config.h`, CubeMX routing and `examples/stm32f446_example_platform.c` only.

## Build / flash / run procedure

1. Create/open the NUCLEO-F446RE project in STM32CubeMX/STM32CubeIDE.
2. Generate the HAL project after applying the routing above.
3. Add the model-local source files listed above.
4. Add the shared ADS1299 core sources required by `ads1299.h`, portable port, frame/model/packet/profile APIs.
5. Add include paths for this model's `board`, `ads1299_port`, `examples` and the shared core.
6. In user code after `MX_GPIO_Init()`, SPI and USART initialization, include `stm32f446_beginner_demo.h` and call:

```c
(void)stm32f446_ads1299_beginner_demo(1000u);
```

7. Build with STM32CubeIDE and flash with the board's ST-LINK.
8. Open the ST-LINK VCP terminal using the baud rate configured in the generated USART2 project.
9. Verify ID/probe, internal-test, input-short and 250-SPS EEG messages before connecting real electrodes.

No particular baud rate is hard-coded by this repository-level procedure; use the value actually configured in the consuming Cube project.

## Expected output / failure classes

Success progresses through ADS1299 ID, internal-test frames, input-short frames, 250-SPS configuration and final completion. The progressive example reports distinct failures for reset/power, SPI/ID, DRDY timeout, frame read and USART transport.

## Host smoke tests

The model-local host test recipe is:

```sh
cd firmware/mcu/01_STMicroelectronics/STM32F446/tests
make -f Makefile.host clean
make -f Makefile.host test
```

This recipe exercises the STM32F446 portable adapter together with shared ADS1299 core sources. Presence of the recipe is not itself evidence that the test has passed.

## SDK ownership and performance boundary

For first 250-SPS bring-up the progressive example intentionally uses a simple blocking HAL path. For sustained higher-rate or multi-device acquisition, use interrupt/DMA-driven acquisition, bounded buffers and explicit overflow accounting. Keep UART/USB/network processing outside the DRDY-critical path.

The older `examples/main_ads1299.c` / `board_ads1299_hal()` path is retained for compatibility; it is no longer the recommended new-student path.
