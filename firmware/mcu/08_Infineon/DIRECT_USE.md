# Infineon MCU Direct ADS1299 Use

This file adds a reusable ADS1299 integration path for the Infineon targets in this folder. Existing model-specific ports and examples remain unchanged.

## Platform binding

Reuse each target's existing `ads1299_port/` abstraction and bind it to the selected SDK:

- PSoC 6: ModusToolbox HAL/PDL SPI, GPIO and interrupt resources.
- XMC4500/XMC4700: DAVE/XMC Lib USIC SPI, GPIO and ERU/event resources.
- XMC7200: ModusToolbox/PDL SCB SPI, GPIO and interrupt/DMA resources.

## ADS1299 setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz for hardware bring-up.
- CS/RESET/PWDN/START are outputs; DRDY is a falling-edge interrupt input.
- One ADS1299-8 continuous frame is exactly 27 bytes.

Recommended flow:

`DRDY -> short ISR/callback -> high-priority acquisition context -> 27-byte SPI read -> ring buffer -> USB/Ethernet/BLE/application`

Do not perform logging, networking or signal processing in the DRDY callback.

## Shared ready-app

Add `08_Infineon/_shared/infineon_ads1299_ready_app.c/.h` plus the common ADS1299 core sources including `ads1299_profiles.c`.

Available profiles:

- `INFINEON_ADS1299_PROFILE_EEG_250`
- `INFINEON_ADS1299_PROFILE_EEG_500`
- `INFINEON_ADS1299_PROFILE_EEG_1000`
- `INFINEON_ADS1299_PROFILE_INTERNAL_TEST`
- `INFINEON_ADS1299_PROFILE_INPUT_SHORT`

## Bring-up sequence

1. Hardware reset and read ADS1299 ID.
2. Internal test profile.
3. Input-short profile.
4. EEG 250 SPS profile.
5. Long continuous capture with header/error/overflow monitoring.
6. Stress the final transport path.
7. Add BIAS, lead-off and faster rates only after baseline stability.

## DMA guidance

First validate a synchronous SPI path. Then enable DMA where useful, with static ping-pong/ring buffers and explicit ownership between DMA completion and consumers. On cached high-performance devices, ensure DMA coherency is handled correctly.

## 64-channel architecture

For 8 x ADS1299, multiple deterministic SPI/DMA lanes or an FPGA capture front-end are preferred when tight cross-device skew matters. The Infineon MCU can then handle control, buffering, DSP and communication.

Pin numbers are intentionally not fixed because board routing and pinmux are board-specific.