# Nationstech N32 + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

Configure the selected N32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz for bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep pins board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On each DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the frame into a fixed-size ring buffer for USB/UART/other transport processing.

Keep the DRDY ISR minimal. Logging, packetization and floating-point conversion belong outside the ISR.

First prove the blocking transfer with the internal test source. Add SPI DMA and ping-pong/ring buffers only after that path is stable. Track SPI errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes when practical. For strict synchronization, use `firmware/fpga/common` as the deterministic acquisition front end and let N32 handle control and transport.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
