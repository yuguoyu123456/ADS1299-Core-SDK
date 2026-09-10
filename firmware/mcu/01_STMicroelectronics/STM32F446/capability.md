# Capability

Current round status: **integration candidate-complete**. Capability presence is not the same as build or hardware validation.

| Item | Current repository capability |
|---|---|
| Reference MCU | STM32F446RET6 |
| Reference board | NUCLEO-F446RE (MB1136) |
| Vendor integration | STM32CubeMX / STM32CubeIDE / STM32CubeF4 HAL recipe present |
| SPI | Reference configuration documented for Mode 1, 8-bit, MSB-first, software CS |
| ADS1299 port | SPI/GPIO/DRDY/delay portable adapter present |
| Beginner flow | Probe/ID -> internal test -> input short -> 250-SPS gain-24 EEG -> stream -> clean stop |
| Host transport | Reference starter path uses USART2/ST-LINK VCP |
| Diagnostics | Reset/power, SPI/ID, DRDY timeout, frame read and transport failure classes |
| Buffering guidance | Static bounded starter queue plus IRQ/DMA migration guidance |
| Multi-ADS1299 | Shared-core architecture can be consumed; this STM32F446 model path is not hardware-validated for multi-device use |
| 64-channel | Guidance only; no STM32F446 8xADS1299 throughput or board validation recorded |

## Scope boundary

ADS1299 register/model/frame/profile behavior remains in the shared core. This model folder owns board routing, HAL binding, examples and model-local tests only.

For sustained acquisition, DMA/interrupt service and bounded queues should isolate DRDY timing from UART/USB/network work. Actual sustainable sample rate, device count, queue depth and transport bandwidth require measurement on the final board.

## Evidence still required

- recorded host-test PASS;
- clean STM32CubeIDE build/link of the progressive reference demo;
- NUCLEO-F446RE + ADS1299 physical ID/internal-test/input-short/EEG run;
- sustained acquisition with overflow/loss accounting;
- multi-ADS1299 timing and transport measurements.
