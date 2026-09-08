# GigaDevice GD32 Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the GD32 targets in this folder. Existing model-specific ports and examples remain unchanged.

## SDK binding

Reuse each target's existing `ads1299_port/` abstraction and bind it to the GD32 firmware library/HAL or the project's low-level SPI/GPIO/EXTI implementation.

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up; use a lower initial clock on low-power/resource-limited targets if needed.
- CS/RESET/PWDN/START outputs; DRDY falling-edge EXTI input.
- ADS1299-8 continuous frame = 27 bytes.

Recommended flow:

`DRDY -> short EXTI ISR -> high-priority acquisition context -> 27-byte SPI transfer -> ring buffer -> USB/UART/Ethernet/Wi-Fi/application`

## Shared ready-app

Add `11_GigaDevice/_shared/gd32_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

Profiles:
- `GD32_ADS1299_PROFILE_EEG_250`
- `GD32_ADS1299_PROFILE_EEG_500`
- `GD32_ADS1299_PROFILE_EEG_1000`
- `GD32_ADS1299_PROFILE_INTERNAL_TEST`
- `GD32_ADS1299_PROFILE_INPUT_SHORT`

## Validation

1. Hardware reset and ADS1299 ID verification.
2. Internal test.
3. Input short.
4. EEG 250 SPS.
5. Sustained capture with frame/header, read-error and overflow monitoring.
6. Stress the final transport path.
7. Add BIAS/lead-off and higher rates only after stability.

## DMA/cache guidance

First establish a synchronous reference path, then add DMA with static ping-pong buffers. On high-performance/cached GD32 variants, use DMA-accessible memory and handle cache coherency correctly.

## 64-channel architecture

For eight ADS1299 devices, multiple deterministic SPI/DMA lanes or an FPGA front-end are preferred for tight cross-device synchronization. GD32 devices can then handle control, aggregation, buffering and communication according to their performance class.

Pin numbers are intentionally board-specific.