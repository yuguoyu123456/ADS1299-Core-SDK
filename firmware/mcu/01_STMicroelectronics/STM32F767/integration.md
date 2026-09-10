# STM32F767 integration with STM32Cube

Reference path: **NUCLEO-F767ZI (MB1137, STM32F767ZIT6)** using STM32CubeMX / STM32CubeIDE and STM32CubeF7 HAL.

## 1. Generate the vendor project

Create or open an STM32Cube project for NUCLEO-F767ZI. Keep vendor startup files, linker scripts, CMSIS, HAL and generated configuration in that project; do not copy the vendor SDK into this repository.

Configure the hardware selections documented in `board/README.md` and `board/board_config.h`:

- SPI1 master, full duplex, software NSS, 8-bit, MSB-first, CPOL=0, CPHA=1;
- PA5/PA6/PA7 for SPI1 SCK/MISO/MOSI;
- PD14 software CS, idle high;
- PD15 DRDY input or EXTI, active low;
- PF12 RESET, PF13 PWDN, PE9 START as GPIO outputs;
- USART3 on PD8/PD9 through the ST-LINK VCP route;
- UART baud must match `ADS1299_BOARD_STREAM_BAUD` in `board/board_config.h`.

## 2. Add repository sources

Add these model-local sources:

```text
ads1299_port/ads1299_spi.c
ads1299_port/ads1299_gpio.c
ads1299_port/ads1299_drdy.c
examples/stm32f767_example_platform.c
examples/stm32f767_beginner_demo.c
```

Add the corresponding model-local include paths and the shared ADS1299 sources/headers required by the APIs used by the beginner demo from:

```text
firmware/core_driver/ads1299/
```

Do not create a reduced or duplicated platform-local ADS1299 register driver. Preserve the shared core as the implementation of commands, register semantics, model handling, frame parsing, packet encoding and multi-device behavior.

The original `examples/main_ads1299.c` may also remain in an existing application; `stm32f767_example_platform.c` provides its legacy `board_ads1299_hal()` binding.

## 3. Call the beginner demo

After Cube-generated clock/GPIO/SPI/UART initialization, include:

```c
#include "stm32f767_beginner_demo.h"
```

Then run:

```c
int rc = stm32f767_ads1299_beginner_demo(1000u);
```

`1000u` produces a finite validation stream before clean STOP/SDATAC. Use `0u` only for an intentionally endless stream.

The demo performs probe/ID -> internal test -> input short -> 250-SPS gain-24 EEG configuration -> DRDY-paced reads -> canonical packet transport -> clean stop.

## 4. Build, flash and run

1. Let STM32CubeIDE regenerate the project after peripheral configuration.
2. Add the repository source files and include paths above to the application target.
3. Perform a clean build in STM32CubeIDE.
4. Flash with the onboard ST-LINK using the normal CubeIDE Run/Debug flow.
5. Open the ST-LINK virtual COM port using the baud in `board/board_config.h`.
6. Confirm the textual probe/self-test messages before interpreting binary stream packets.

A successful source integration is not the same as recorded BUILD-VERIFIED evidence. Only update `validation.md` to BUILD-VERIFIED after an actual clean build has succeeded with a documented toolchain/project configuration.

## 5. Diagnostics

- RESET/PWDN failure: check power and control GPIO routing.
- ID/SDATAC failure: check SPI Mode 1, software CS and wiring.
- DRDY timeout: check PD15 routing/EXTI and conversion start state.
- frame-read failure: check CS/SPI integrity during continuous conversion.
- stream-transport failure: check USART3 VCP routing and configured baud.

## 6. Porting to another STM32F767 board

Do not edit the shared ADS1299 core. Normally change only:

1. STM32CubeMX pin/peripheral routing;
2. `board/board_config.h`;
3. `examples/stm32f767_example_platform.c` only if generated HAL peripheral handle names or board glue differ.

## 7. Host smoke test

Without target hardware, the model-local integration recipe is:

```sh
cd firmware/mcu/01_STMicroelectronics/STM32F767/tests
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line after a successful execution:

```text
PASS STM32F767 integration smoke tests
```

A host PASS is software integration evidence only; it does not establish STM32Cube or board verification.

## Sustained-acquisition boundary

The beginner demo uses blocking HAL calls for inspectable first bring-up. Sustained/high-rate or multi-device designs should use IRQ/DMA acquisition into bounded buffering with overflow counters, and should drain UART/USB/Ethernet outside the DRDY timing path. STM32F7 DMA buffers in cacheable SRAM need an explicit cache-coherency strategy.
