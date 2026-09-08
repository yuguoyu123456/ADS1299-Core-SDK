# Artery AT32 + ADS1299 Direct-Use Guide

This guide adds a ready application path without replacing any existing model-specific `ads1299_port` implementation.

## Board configuration

Configure the selected AT32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 SPI mode 1 (CPOL=0, CPHA=1). During first bring-up use a conservative SCLK around 2-4 MHz.

Configure these board signals:

- CS: GPIO output, inactive high
- RESET: GPIO output
- PWDN: GPIO output
- START: GPIO output
- DRDY: GPIO input with falling-edge EXINT

Keep all pin numbers board-specific. Do not hard-code one reference-board mapping into the portable driver.

## Source files

Include the existing model port plus:

- `firmware/core_driver/ads1299/ads1299*.c`
- `firmware/core_driver/ads1299/ads1299_profiles.c`
- `firmware/mcu/13_Artery/_shared/artery_ads1299_ready_app.c`

## Recommended runtime flow

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callback object.
3. Call `ads1299_init()` and read the device ID.
4. Run `ARTERY_ADS1299_PROFILE_INTERNAL_TEST`.
5. Run `ARTERY_ADS1299_PROFILE_INPUT_SHORT`.
6. Apply `ARTERY_ADS1299_PROFILE_EEG_250`.
7. Start RDATAC streaming.
8. On every DRDY falling edge, service exactly one complete ADS1299-8 frame: 27 bytes.
9. Push the completed frame into a ring buffer for USB/UART/Ethernet processing.

Keep the DRDY ISR minimal. It should only capture timing/sequence information and signal the acquisition context. Do not print, packetize or perform floating-point conversion inside the ISR.

## DMA

First prove the blocking SPI path with the ADS1299 internal test source. After the signal is correct and repeatable, move the 27-byte frame transfer to DMA when the selected AT32 family supports a suitable DMA mapping. Use ping-pong or ring buffering and track overrun counters.

## 64-channel architecture

For 8 x ADS1299, independent CS control and multiple SPI/DMA lanes can be used when timing allows. If low inter-device skew is a hard requirement, use `firmware/fpga/common` for deterministic frame capture and let the AT32 MCU handle control, buffering and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
