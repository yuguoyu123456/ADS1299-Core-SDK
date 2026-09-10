# Integration with STM32CubeG0 / STM32CubeIDE

Reference board: **NUCLEO-G071RB (MB1360, STM32G071RBT6)**.

Validation status: **TEMPLATE / repository integration present**. The recipe below documents the intended Cube project composition; this repository does not yet record a clean target build or physical ADS1299 run for this exact configuration.

## CubeMX / CubeIDE setup

Create or open an STM32G071RBT6 project for NUCLEO-G071RB. Configure:

- SPI1 master, full duplex, 8-bit, MSB-first, **CPOL low / CPHA second edge (SPI Mode 1)**;
- PA5 SPI1_SCK, PA6 SPI1_MISO, PA7 SPI1_MOSI;
- PB0 ADS1299 CS output, idle high;
- PC7 ADS1299 DRDY input for the starter polling example;
- PA9 RESET output, PA8 PWDN output, PB14 START output;
- USART2 PA2/PA3 at 460800 baud, 8-N-1, for the ST-LINK VCP starter stream.

Use startup files, linker scripts, HAL/CMSIS and generated initialization from the installed STM32CubeG0 package. Do not copy invented vendor project files from this repository.

## Sources to add

Model-local sources:

```text
firmware/mcu/01_STMicroelectronics/STM32G071/ads1299_port/ads1299_spi.c
firmware/mcu/01_STMicroelectronics/STM32G071/ads1299_port/ads1299_gpio.c
firmware/mcu/01_STMicroelectronics/STM32G071/ads1299_port/ads1299_drdy.c
firmware/mcu/01_STMicroelectronics/STM32G071/examples/stm32g071_example_platform.c
firmware/mcu/01_STMicroelectronics/STM32G071/examples/stm32g071_beginner_demo.c
```

Shared SDK sources:

```text
firmware/core_driver/ads1299/ads1299.c
firmware/core_driver/ads1299/ads1299_frame.c
firmware/core_driver/ads1299/ads1299_model.c
firmware/core_driver/ads1299/ads1299_multi.c
firmware/common/data_packet/ads1299_packet.c
```

Keep any additional shared ADS1299 modules already required by the current core build; do not exclude existing profiles, self-tests, high-level/control layers or other established shared modules merely to simplify a local project.

Add include paths for the shared core, shared packet layer, model `ads1299_port`, `examples`, and `board` directories.

## One obvious board/config point

Repository-owned pins, starter transport and timing choices live in `board/board_config.h`. When another STM32G071 board uses different CubeMX routing, normally change only CubeMX pin/peripheral assignment, `board/board_config.h`, and the model-local handle binding in `examples/stm32g071_example_platform.c` if generated handle names differ.

Do **not** edit shared ADS1299 register/model/core files for normal first bring-up.

## Run sequence

After normal Cube initialization, call:

```c
stm32g071_ads1299_beginner_demo(1000u);
```

The progressive path performs reset/SDATAC, family ID probe, internal-test acquisition, input-short acquisition, 250-SPS gain-24 EEG configuration, DRDY/read, canonical packet streaming, then clean STOP/SDATAC for a finite run. Use `0u` only for intentional continuous streaming.

## Expected success progression

```text
ADS1299 STM32G071 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4/-6 report their matching family ID. Once canonical binary packets start, the stream is no longer purely text.

## Diagnostics

- RESET/PWDN failure: inspect power and control GPIO routing.
- ID/SDATAC failure: inspect SPI Mode 1, CS, clock and MISO/MOSI wiring.
- DRDY timeout: inspect conversion state and PC7 routing.
- frame read failure: inspect SPI integrity after DRDY.
- transport failure: inspect USART2/VCP configuration and baud rate.

## Small-MCU sustained-acquisition rule

The blocking path is for first bring-up. For sustained acquisition, keep the DRDY/EXTI path minimal, enqueue raw frames into a fixed-size bounded queue/ring, process/packetize outside the timing-critical path, and count overflow explicitly. Avoid heap allocation and floating-point conversion in acquisition timing code.

The canonical stream is 49 bytes/frame; at 250 SPS that is 12,250 payload bytes/s, so 115200-baud 8-N-1 is insufficient for an unthrottled full stream. The starter configuration therefore uses 460800 baud.
