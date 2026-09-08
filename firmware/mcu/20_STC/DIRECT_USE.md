# STC32 + ADS1299 Direct-Use Guide

This guide is additive and preserves the existing `ads1299_port` implementation.

Configure STC32 SPI as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz during bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep actual pins board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On each DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the frame into a fixed-size ring buffer for UART/USB/other transport processing.

Keep the DRDY ISR minimal. Logging, packetization and floating-point conversion belong outside the ISR. First prove the blocking transfer with the internal test source, then add DMA only if the selected STC32 peripheral path supports it and the blocking path is already stable.

For 8 x ADS1299 / 64 channels, use STC32 primarily as a control/interface controller unless enough independent capture resources are available. For strict synchronization, use `firmware/fpga/common` as the deterministic acquisition front end.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> multi-device synchronization`.
