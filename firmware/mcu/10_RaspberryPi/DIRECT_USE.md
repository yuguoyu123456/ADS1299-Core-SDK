# Raspberry Pi MCU Direct ADS1299 Use

This file adds a reusable Pico SDK integration path for RP2040 and RP2350. Existing model-specific ports and examples remain unchanged.

## Pico SDK binding

Reuse each target's `ads1299_port/` abstraction and bind it to Pico SDK `spi_*`, GPIO interrupt and timing functions.

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START outputs; DRDY falling-edge GPIO interrupt input.
- ADS1299-8 continuous frame = 27 bytes.

Recommended flow:

`DRDY -> short GPIO callback -> high-priority acquisition context/core -> 27-byte SPI read -> ring buffer -> USB/UART/network coprocessor/application`

## Shared helper

Add `10_RaspberryPi/_shared/rp_ads1299_ready_app.c/.h` plus the common ADS1299 core sources including `ads1299_profiles.c`.

Profiles:
- `RP_ADS1299_PROFILE_EEG_250`
- `RP_ADS1299_PROFILE_EEG_500`
- `RP_ADS1299_PROFILE_EEG_1000`
- `RP_ADS1299_PROFILE_INTERNAL_TEST`
- `RP_ADS1299_PROFILE_INPUT_SHORT`

## Dual-core and DMA guidance

First validate a simple blocking SPI transfer. Then add DMA with fixed ping-pong buffers if needed. One core can own deterministic ADS1299 acquisition while the other handles USB/application work, but ownership of SPI and active buffers must remain unambiguous.

PIO can be useful for specialized deterministic interfaces, but the standard hardware SPI path should be validated first; do not add PIO complexity unless the design has a measured reason.

## Validation

1. Hardware reset and ID read.
2. Internal test.
3. Input short.
4. EEG 250 SPS.
5. Long capture with transport idle.
6. Repeat under USB/UART/application load while monitoring read errors and queue overflow.
7. Add BIAS/lead-off and higher rates only after stability.

## 64-channel architecture

For eight ADS1299 devices, an FPGA or multiple independent SPI/DMA lanes are preferred when tight cross-device skew matters. RP2040/RP2350 can then handle control, buffering and USB/host transport.

Pin numbers are intentionally board-specific.