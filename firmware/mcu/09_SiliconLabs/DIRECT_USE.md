# Silicon Labs MCU Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the Silicon Labs targets in this folder. Existing model-specific ports and examples remain unchanged.

## Platform binding

Reuse each target's `ads1299_port/` abstraction and bind it to the selected Gecko SDK / emlib / Silicon Labs peripheral layer:

- EFM32 / EFM32GG11: USART synchronous master, GPIO interrupt and optional LDMA resources.
- EFR32BG22 / EFR32MG24 / EFR32MG26: Gecko SDK USART/SPIDRV or low-level USART synchronous mode, GPIO interrupt and LDMA resources.

## ADS1299 requirements

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge interrupt input.
- ADS1299-8 continuous frame = 27 bytes.

Recommended path:

`DRDY -> short GPIO callback -> high-priority acquisition context -> 27-byte SPI transfer -> ring buffer -> BLE/Thread/USB/UART/application`

Keep wireless stacks, logging and DSP outside the DRDY callback.

## Shared ready-app

Add `09_SiliconLabs/_shared/silabs_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

Profiles:
- `SILABS_ADS1299_PROFILE_EEG_250`
- `SILABS_ADS1299_PROFILE_EEG_500`
- `SILABS_ADS1299_PROFILE_EEG_1000`
- `SILABS_ADS1299_PROFILE_INTERNAL_TEST`
- `SILABS_ADS1299_PROFILE_INPUT_SHORT`

## Validation

1. Hardware reset and ID read.
2. Internal test.
3. Input short.
4. EEG 250 SPS.
5. Sustained capture with radio idle.
6. Repeat under representative radio/transport load while monitoring read errors and queue overflow.
7. Add BIAS/lead-off and faster rates only after baseline stability.

## DMA and low-power guidance

First prove the synchronous transfer path. Then use LDMA where available with static ping-pong buffers and explicit ownership. If low-power modes are used, verify wake latency and ensure DRDY/SPI service remains deterministic.

## 64-channel architecture

For 8 x ADS1299, prefer multiple deterministic SPI/DMA lanes or FPGA capture when tight inter-device skew matters. Silicon Labs wireless MCUs can then handle control and wireless transport after deterministic acquisition is established.

Pin numbers are intentionally board-specific.