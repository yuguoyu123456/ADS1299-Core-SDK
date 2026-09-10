# STM32H563 ADS1299 board configuration

## Reference path

Use **NUCLEO-H563ZI (MB1404, STM32H563ZIT6)** as the reference board for the standardized STM32H563 beginner path.

Edit only `board_config.h` for board-dependent pin/peripheral choices. Normal bring-up must not require edits to the shared ADS1299 driver.

## Starter wiring

| ADS1299 signal | NUCLEO-H563ZI MCU pin | Board role |
|---|---:|---|
| SCLK | PA5 | Zio D13 / SPI1_SCK |
| DOUT / MISO | PG9 | Zio D12 / SPI1_MISO |
| DIN / MOSI | PB5 | Zio D11 / SPI1_MOSI |
| CS | PD14 | Zio D10 |
| DRDY | PD15 | Zio D9, active low |
| RESET | PF3 | Zio D8, active low |
| PWDN | PG2 | Zio D49, active low |
| START | PG3 | Zio D50 |
| Host TX | PD8 | USART3_TX / ST-LINK VCP |
| Host RX | PD9 | USART3_RX / ST-LINK VCP |

The PA5/PG9/PB5 SPI1 route is intentional. ST documents it as a usable SPI1 combination on NUCLEO-H563ZI; it avoids making the beginner path depend on the PA7 solder-bridge change associated with an alternate SPI1 MOSI route.

Connect digital ground between the Nucleo board and ADS1299 hardware. Confirm that ADS1299 DVDD/digital-I/O levels are compatible with the 3.3 V STM32H5 GPIO domain before wiring. This folder does not define the analog front-end power, electrode connection, BIAS safety, isolation, EMC, or medical-device design.

## STM32CubeH5 configuration

For the reference mapping:

1. Select `NUCLEO-H563ZI` / `STM32H563ZIT6` in STM32CubeMX or STM32CubeIDE.
2. Configure SPI1 as full-duplex master, 8-bit, **CPOL Low / CPHA second edge (SPI Mode 1)**, MSB first.
3. Configure PA5 / PG9 / PB5 for SPI1 SCK / MISO / MOSI.
4. Configure PD14 as push-pull output for software CS and initialize it high.
5. Configure PD15 as DRDY input. Begin with polling; for sustained acquisition use falling-edge EXTI and keep the ISR short.
6. Configure PF3 RESET, PG2 PWDN and PG3 START as push-pull outputs.
7. Configure USART3 PD8/PD9 for the on-board ST-LINK virtual COM path when logging/streaming is needed. `460800 8N1` is the starter transport setting in `board_config.h`.
8. Generate the STM32CubeH5 project and bind the generated HAL handles/GPIO operations through `../ads1299_port/`. Do not edit shared ADS1299 register/core files for board bring-up.

Use startup files, linker scripts and system files generated/provided by the selected STM32CubeH5 toolchain; this repository does not invent replacements for them.

## Why 460800 baud

The repository canonical packet is 49 bytes. At 250 samples/s the stream is 12,250 bytes/s before optional log text. A conventional 115200-baud 8N1 UART carries about 11,520 payload bytes/s in the ideal case, so it is not sufficient for an uninterrupted canonical stream. 460800 baud provides useful margin for the starter path.

## Sustained acquisition rule

Blocking HAL calls are acceptable for probe and early self-test bring-up. For sustained EEG use the model below:

`DRDY falling edge -> short ISR / timestamp -> bounded static queue or ring buffer -> frame handling -> packetization -> host transport`

Do not perform long UART/USB/network work inside the DRDY ISR. Count queue overruns explicitly. If SPI DMA is introduced, keep CS and frame boundaries deterministic and reuse DMA buffers only after completion; apply cache/memory maintenance only when required by the actual STM32H563 memory and Cube configuration.

## Another STM32H563 board

Keep the shared ADS1299 core and the model port API unchanged. A custom board normally changes only `board_config.h` plus CubeMX-generated peripheral/pin setup: SPI instance/pins, CS/DRDY/RESET/PWDN/START GPIOs, clocks and optional host transport.

## Validation status

- **TEMPLATE / integration configuration present**: yes.
- **STM32CubeH5 build verified for this exact reference configuration**: not yet recorded.
- **NUCLEO-H563ZI + ADS1299 board verified**: not yet recorded.
- **Sustained EXTI/DMA acquisition verified**: not yet recorded.

Do not interpret this file as bench, electrical-safety, EMC, production or regulatory validation.

## Primary board sources

Check final wiring against STMicroelectronics **UM3115, STM32H5 Nucleo-144 board (MB1404)** and the current NUCLEO-H563ZI schematic/board-design files. UM3115 identifies the NUCLEO-H563ZI pin assignments and Zio connector routing. ST's board guidance also documents the PA5/PG9/PB5 SPI1 route and the default USART3 PD8/PD9 ST-LINK VCP path used here.
