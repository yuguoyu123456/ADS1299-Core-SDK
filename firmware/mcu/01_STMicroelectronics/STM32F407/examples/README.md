# STM32F407 ADS1299 examples

Reference path: **STM32F407G-DISC1 / MB997 / STM32F407VGT6**, STM32CubeMX + STM32CubeIDE.

Validation status: **TEMPLATE / repository integration present**. These sources have not yet been recorded as STM32CubeIDE BUILD-VERIFIED or physical BOARD-VERIFIED with ADS1299 hardware.

## Beginner entry point

Use `stm32f407_ads1299_beginner_demo(stream_frames)` from `stm32f407_beginner_demo.c`.

- `stream_frames = 1000u`: run the full bring-up sequence and stream 1000 canonical packets.
- `stream_frames = 0u`: stream continuously until reset/application stop.

The sequence is intentionally progressive:

1. create the STM32F407 HAL binding;
2. initialize the shared `ads1299_port_t` based driver;
3. hardware RESET and SDATAC;
4. read and validate ADS1299-4/-6/-8 ID;
5. acquire a short internal-test run;
6. acquire a short input-short run;
7. configure normal-input EEG at 250 SPS, gain 24;
8. wait for active-low DRDY and read continuous frames;
9. encode the repository canonical 49-byte packet and send it through the configured UART;
10. STOP + SDATAC for a clean finite exit.

Shared ADS1299 register behavior remains in `firmware/core_driver/ads1299`. Do not copy register literals into this folder.

## Files to add to a Cube project

Add these model files:

- `../ads1299_port/ads1299_spi.c`
- `../ads1299_port/ads1299_gpio.c`
- `../ads1299_port/ads1299_drdy.c`
- `stm32f407_example_platform.c`
- `stm32f407_beginner_demo.c`

Also add the shared ADS1299 core sources required by the repository build, including the driver/frame/model/packet modules used by the example. Use the shared headers from `firmware/core_driver/ads1299` rather than making local copies.

## CubeMX configuration

Start from the STM32F407G-DISC1 / STM32F407VGT6 project and follow `../board/board_config.h`.

Configure:

- SPI1 master, full duplex, 8-bit, MSB-first;
- CPOL Low, CPHA 2Edge = SPI Mode 1;
- GPIO outputs labelled `ADS1299_CS`, `ADS1299_RESET`, `ADS1299_PWDN`, `ADS1299_START`;
- GPIO input labelled `ADS1299_DRDY`;
- USART2 TX when `ADS1299_BOARD_USE_UART_STREAM` is enabled.

The reference software mapping is documented in `../board/board_config.h`. On MB997, PA5/PA6/PA7 interact with board-level circuitry on some revisions; verify the exact board schematic/solder-bridge state before attaching ADS1299. If another pin/peripheral assignment is needed, change CubeMX and the board/config layer only.

## Application call

After Cube-generated peripheral initialization, call for example:

```c
#include "stm32f407_beginner_demo.h"

int rc = stm32f407_ads1299_beginner_demo(1000u);
if (rc != 0) {
    Error_Handler();
}
```

Do not place the demo before GPIO/SPI/UART initialization.

## Expected diagnostic path

With a correctly powered and wired ADS1299-family device, the text path should progress through messages equivalent to:

```text
ADS1299 STM32F407 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical 49-byte packets
```

For ADS1299-4 or ADS1299-6 the ID line changes accordingly.

Failures are separated so a student can localize bring-up problems:

- `ERR RESET` -> power/PWDN/RESET path;
- `ERR SDATAC` or `ERR ID` -> SPI Mode 1, CS, clock, power, DOUT/DIN;
- `ERR DRDY timeout` -> DRDY wiring/configuration or conversion not running;
- `ERR frame read` -> SPI/CS/data path;
- `ERR stream transport` -> UART/host transport configuration.

## Timing and high-performance note

The starter `delay_us` implementation rounds up to the HAL millisecond tick. It is deliberately simple for bring-up. Replace only the platform delay callback with a timer/DWT implementation when precise sub-millisecond timing is required.

For sustained high-rate or multi-ADS1299 operation on STM32F407, do not leave UART or other host transport blocking inside the acquisition timing path. Use DMA/interrupt-driven capture and the bounded queue policy documented in `../board/board_config.h`; add explicit overflow accounting rather than allowing an unbounded buffer.

## Moving to another STM32F407 board

Normally change only:

1. CubeMX pin/peripheral routing;
2. `../board/board_config.h` handle/pin macros if the generated names differ.

Do not edit `ads1299.c`, ADS1299 register definitions, model logic, or shared frame/packet code merely to move between STM32F407 boards.
