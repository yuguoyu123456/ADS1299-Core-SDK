# Ambiq Apollo + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing model-specific `ads1299_port` file.

Configure the selected Ambiq IOM/SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz for bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep actual pin assignments board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI/IOM.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On every DRDY event, service exactly one complete ADS1299-8 frame (27 bytes).
9. Push the frame into a bounded ring buffer for BLE/UART/USB/other transport work.

Keep the DRDY ISR minimal. Low-power state transitions, radio work, logging and floating-point conversion must stay outside the timing-critical path.

First prove the blocking transfer with the internal test source. Then use DMA where available and keep wake-to-service latency bounded. Track SPI errors, sequence gaps, late service and buffer overflow.

For 8 x ADS1299 / 64 channels, use Ambiq primarily as a low-power controller/gateway unless sufficient independent capture resources are available. For strict synchronization, use `firmware/fpga/common` as the deterministic front end.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> low-power wake/sleep stress -> lead-off/BIAS -> transport stress -> multi-device synchronization`
