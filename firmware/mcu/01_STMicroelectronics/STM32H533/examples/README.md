# STM32H533 ADS1299 examples

Reference path: **NUCLEO-H533RE (STM32H533RET6 / MB1814)** with STM32CubeH5 HAL.

Validation status: **TEMPLATE / repository integration present**. These sources have not yet been recorded as BUILD-VERIFIED in STM32CubeIDE and have not been BOARD-VERIFIED with physical NUCLEO-H533RE + ADS1299 hardware.

## Beginner entry point

Call:

```c
stm32h533_ads1299_beginner_demo(1000u);
```

Use `0u` instead of `1000u` for continuous streaming.

The progressive demo performs:

1. portable HAL/port initialization;
2. hardware RESET and SDATAC;
3. ADS1299-family ID/probe;
4. internal-test capture;
5. input-short capture;
6. 250-SPS, gain-24 normal-input configuration;
7. continuous acquisition;
8. canonical 49-byte repository packet encoding and USART2/ST-LINK VCP streaming;
9. clean STOP + SDATAC when a finite frame count completes.

The old `main_ads1299.c` remains available as a minimal compatibility example. `stm32h533_example_platform.c` implements the same `board_ads1299_hal()` callback expected by that file.

## One board/config file

For the reference board, hardware-dependent values live in:

```text
../board/board_config.h
```

The starter mapping is SPI1 PA5/PA6/PA7, CS PC9, DRDY PC6, RESET PC7, PWDN PA8, START PB10, and USART2 PA2/PA3 to the ST-LINK virtual COM port. For another STM32H533 board, change the CubeMX pin/peripheral configuration and board aliases; do not edit the shared ADS1299 driver/register/model files.

## CubeMX / STM32CubeIDE integration

Create or open an STM32CubeH5 project for NUCLEO-H533RE and configure the reference path as follows:

- SPI1 master, 8-bit, MSB-first, **CPOL=Low and CPHA=2 Edge (SPI Mode 1)**; use software-controlled chip select.
- PA5 SPI1_SCK, PA6 SPI1_MISO, PA7 SPI1_MOSI.
- PC9 CS output, PC7 RESET output, PA8 PWDN output, PB10 START output.
- PC6 DRDY input for polling bring-up; for sustained acquisition it should become a falling-edge EXTI source with minimal ISR work.
- USART2 PA2/PA3 for ST-LINK VCP at the board-configured starter baud rate.

Add these model-local sources:

```text
ads1299_port/ads1299_spi.c
ads1299_port/ads1299_gpio.c
ads1299_port/ads1299_drdy.c
examples/stm32h533_example_platform.c
examples/stm32h533_beginner_demo.c
```

Also add the shared ADS1299 implementation already used by the repository, including the required core sources and:

```text
firmware/common/data_packet/ads1299_packet.c
```

Add include paths for the STM32H533 `ads1299_port`, `board`, and `examples` folders, the shared `firmware/core_driver/ads1299` folder, and `firmware/common/data_packet`.

After Cube-generated HAL initialization, call the beginner demo from the application section of `main()`.

## Expected first-run text

Before binary packet streaming begins, a correct bring-up should progress through messages equivalent to:

```text
ADS1299 STM32H533 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4 and ADS1299-6 IDs are accepted too and the channel loop follows the detected variant.

## Diagnostics

- `ERR RESET...`: inspect power, PWDN and RESET wiring/configuration.
- `ERR SDATAC...` or `ERR ID...`: inspect SPI Mode 1, clock, CS, power and ADS1299 clock source.
- `ERR DRDY timeout...`: inspect DRDY polarity/wiring and whether conversion has started.
- `ERR frame read...`: inspect SPI/CS integrity and frame timing.
- `ERR stream transport...`: inspect USART2/VCP configuration and host baud rate.

The demo intentionally separates these failure classes so a first-time user does not need to debug raw ADS1299 register bytes.

## Sustained acquisition note

The blocking HAL path is suitable for first bring-up and finite examples. For sustained EEG on STM32H533, prefer a short DRDY falling-edge ISR, bounded static queue/ring buffer, and packetization/transport outside the timing-critical path. If SPI DMA is introduced, preserve deterministic CS and complete ADS1299 frame boundaries, count overflow explicitly, and apply cache maintenance only where the actual H533 memory/cache configuration requires it.

## Packet contract

Streaming uses the repository shared `ads1299_packet_encode()` implementation rather than a model-local packet format. The canonical packet is 49 bytes and carries sync `0xA5 0x5A`, version/flags, sequence, timestamp, three status bytes, eight signed 32-bit channels, and CRC16.
