# MindMotion MM32 + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

Configure the selected MM32 SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz for bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep actual pins board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On each DRDY event, service one full ADS1299-8 frame (27 bytes).
9. Enqueue the frame into a fixed-size ring buffer for USB/UART/wireless processing.

Keep the ISR short. Do not perform logging, packetization or floating-point conversion inside it.

First validate the blocking transfer using the internal test source. Then add SPI DMA where the chosen MM32 family provides a suitable DMA route. Track SPI errors, sequence gaps, late servicing and queue overflow.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes when practical. For tight inter-device timing, use `firmware/fpga/common` as the deterministic capture front end and let MM32 handle control and transport.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
