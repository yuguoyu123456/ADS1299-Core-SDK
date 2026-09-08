# HDSC HC32 + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

## Board configuration

Configure the selected HC32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 SPI mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz during first bring-up.

GPIO signals:

- CS: output, inactive high
- RESET: output
- PWDN: output
- START: output
- DRDY: falling-edge interrupt input

Keep pin assignments board-specific.

## Runtime flow

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callback object.
3. Initialize the ADS1299 core and read the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On each DRDY event, read exactly one ADS1299-8 frame (27 bytes).
9. Push the completed frame into a ring/ping-pong buffer for transport.

Keep the DRDY ISR short. Logging, packetization and floating-point conversion belong outside the timing-critical acquisition path.

## DMA

First validate a blocking 27-byte transfer using the ADS1299 internal test signal. Then use DMA when the selected HC32 family provides a suitable SPI/DMA route. Track sequence gaps, SPI errors, late service and buffer overflow.

## 64-channel architecture

For 8 x ADS1299, use multiple SPI/DMA lanes when practical. For tight inter-device synchronization, prefer `firmware/fpga/common` for deterministic capture and use HC32 for configuration, buffering and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
