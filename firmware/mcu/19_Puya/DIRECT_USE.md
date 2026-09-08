# Puya PY32 + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

Configure the selected PY32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz for bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep pins board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On every DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the frame into a fixed-size ring buffer for USB/UART/other transport processing.

Keep the DRDY ISR minimal. Logging, packetization and floating-point conversion belong outside the ISR.

First prove the blocking transfer with the internal test source. Add DMA and ping-pong/ring buffers only after that path is stable and only on devices with a suitable SPI/DMA route. Track SPI errors, sequence gaps, late service and buffer overflow.

For 8 x ADS1299 / 64 channels, use more capable multi-SPI/DMA hardware or the repository FPGA capture layer for deterministic aggregation. PY32 devices can still be used for control/interface roles.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS where practical -> lead-off/BIAS -> transport stress -> multi-device synchronization`
