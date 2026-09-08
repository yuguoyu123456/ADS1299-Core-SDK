# Bouffalo Lab BL-Series + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

Configure the selected BL-Series SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz for bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep actual pins board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On every DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the frame into a bounded ring buffer for Wi-Fi/BLE/USB/UART processing.

Keep DRDY service independent from radio/network work. The interrupt should only capture sequence/timing information and signal acquisition work. First prove blocking SPI with the internal test source, then add DMA/non-blocking transfers. Track SPI errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64 channels, separate deterministic acquisition from Wi-Fi/BLE/media tasks. Use multiple SPI/DMA lanes where available; for strict synchronization, prefer `firmware/fpga/common` as the deterministic capture front end.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> network/radio stress -> lead-off/BIAS -> multi-device synchronization`
