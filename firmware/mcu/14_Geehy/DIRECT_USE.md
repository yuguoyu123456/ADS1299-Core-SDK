# Geehy APM32 + ADS1299 Direct-Use Guide

This guide is additive and keeps every existing model-specific `ads1299_port` file unchanged.

## Board configuration

Configure the selected APM32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 SPI mode 1 (CPOL=0, CPHA=1). During first bring-up, start around 2-4 MHz.

Configure these signals:

- CS: output, inactive high
- RESET: output
- PWDN: output
- START: output
- DRDY: falling-edge interrupt input

Keep pin assignments board-specific.

## Source files

Include the existing model port plus:

- `firmware/core_driver/ads1299/ads1299*.c`
- `firmware/core_driver/ads1299/ads1299_profiles.c`
- `firmware/mcu/14_Geehy/_shared/geehy_ads1299_ready_app.c`

## Recommended runtime flow

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callback object.
3. Call `ads1299_init()` and read the device ID.
4. Run `GEEHY_ADS1299_PROFILE_INTERNAL_TEST`.
5. Run `GEEHY_ADS1299_PROFILE_INPUT_SHORT`.
6. Apply `GEEHY_ADS1299_PROFILE_EEG_250`.
7. Start RDATAC streaming.
8. On each DRDY falling edge, service exactly one full ADS1299-8 frame: 27 bytes.
9. Push the frame into a fixed-size ring buffer for USB/UART/other transport processing.

Keep the DRDY ISR minimal. Do not print, packetize or perform floating-point conversion in the ISR.

## DMA and buffering

First validate the blocking transfer using the internal test source. Then, on devices with suitable DMA resources, move the 27-byte transfer to DMA and use ping-pong or ring buffers. Track SPI errors, sequence gaps, late-service events and buffer overflow.

## 64-channel architecture

For 8 x ADS1299, multiple SPI/DMA lanes can be used when timing allows. When low inter-device skew is required, use `firmware/fpga/common` for deterministic capture and let the APM32 MCU handle control and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
