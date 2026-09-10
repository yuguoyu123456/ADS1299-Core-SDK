# STM32H563 ADS1299 examples

Reference path: **NUCLEO-H563ZI (STM32H563ZIT6) + STM32CubeH5 + shared ADS1299 core**.

Validation status: **TEMPLATE / integration example present**. The example source and integration path are present, but a recorded STM32CubeH5 target build and physical-board run are not yet claimed.

## Recommended beginner entry point

Use:

```c
#include "stm32h563_beginner_demo.h"

(void)stm32h563_ads1299_beginner_demo(1000u);
```

Pass `0u` instead of `1000u` for continuous streaming.

The progressive flow is:

1. construct the STM32H563 portable HAL/port adapter;
2. hardware reset and `SDATAC`;
3. read and validate ADS1299-4/-6/-8 device ID;
4. acquire a short internal-test run;
5. acquire a short input-short run;
6. configure normal-input EEG at 250 SPS, gain 24;
7. start continuous conversion and wait for active-low DRDY;
8. read shared-core frames and encode the repository canonical packet;
9. stream packets over the reference USART3/ST-LINK virtual COM path;
10. stop conversion and return to `SDATAC` cleanly.

ADS1299 behavior remains in the shared core. This folder only supplies STM32CubeH5 board/HAL integration and the runnable flow.

## One board/config file to edit

For the reference board use:

`../board/board_config.h`

For another STM32H563 board, keep the shared ADS1299 driver unchanged and normally change only that board/config file plus the matching STM32CubeMX-generated pin/peripheral setup.

The reference configuration expects:

- SPI1 in Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first;
- PA5 SPI1_SCK, PG9 SPI1_MISO, PB5 SPI1_MOSI;
- PD14 CS, PD15 DRDY, PF3 RESET, PG2 PWDN, PG3 START;
- USART3 on PD8/PD9 for ST-LINK VCP starter transport;
- 250 SPS / gain 24 for the normal-input EEG example.

## STM32CubeH5 project setup

Create or open a normal STM32CubeIDE / STM32CubeMX project for NUCLEO-H563ZI. Generate the normal ST startup, linker, HAL and system files with STM32Cube tools; this repository does not invent replacements for those vendor-generated files.

Configure SPI1 as master, full duplex, 8-bit, MSB first, CPOL Low and CPHA second edge. Configure the control GPIO directions described in `board_config.h`; DRDY is an input and may initially be polled. Configure USART3 for the host/VCP path if packet streaming is required.

Add these model-local files to the application build:

- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32h563_example_platform.c`
- `examples/stm32h563_beginner_demo.c`

Also add the shared ADS1299 core modules required by the repository API, including the base driver, frame/model support and canonical packet implementation. Reuse the repository's existing shared sources rather than copying register code into this model directory.

Add include paths for the STM32H563 `ads1299_port`, `board`, `examples`, and shared ADS1299 core headers.

The platform adapter expects Cube-generated global handles named `hspi1` and `huart3`. If your generated project deliberately uses other peripheral instances/handle names, update the board/application integration layer rather than the shared ADS1299 core.

## Expected text output

Before binary packet streaming begins, a correctly wired bring-up should progress through messages similar to:

```text
ADS1299 STM32H563 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4 and ADS1299-6 report their respective ID names. Once canonical binary packets start, do not expect the serial stream to remain readable as plain text.

Typical diagnostics are intentionally separated:

- `ERR RESET` -> inspect RESET/PWDN/power wiring;
- `ERR SDATAC` or `ERR ID` -> inspect SPI Mode 1, CS, device clock and power;
- `ERR DRDY timeout` -> inspect DRDY wiring/configuration and conversion state;
- `ERR frame read` -> inspect SPI/CS timing and frame transfer;
- `ERR stream transport` -> inspect USART3/VCP configuration.

## Sustained acquisition

The polling/blocking example is appropriate for first bring-up and functional validation. For sustained EEG acquisition on STM32H563, evolve the timing path to:

`DRDY falling edge -> short ISR/timestamp -> bounded static queue/ring buffer -> frame/packet work -> transport`

Do not perform long UART/USB/network work inside the DRDY ISR. Count queue overflow explicitly. If SPI DMA is introduced, preserve deterministic CS/frame boundaries and do not reuse a DMA buffer before completion. Apply cache maintenance only according to the actual H563 memory/cache/DMA placement used by the generated project.

## Validation status

- Source/integration example: **PRESENT**
- Probe/internal-test/input-short/250-SPS/stream flows: **PRESENT**
- Shared canonical packet usage: **PRESENT**
- STM32CubeH5 target build: **NOT YET RECORDED**
- NUCLEO-H563ZI + ADS1299 physical board run: **NOT YET RECORDED**
- Sustained EXTI/DMA acquisition: **NOT YET VERIFIED**
- Multi-ADS1299 / 64-channel hardware path: **NOT YET VERIFIED**
