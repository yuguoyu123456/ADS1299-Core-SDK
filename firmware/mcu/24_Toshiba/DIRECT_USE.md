# Toshiba TXZ + ADS1299 Direct-Use Guide

This guide is additive and preserves every existing TXZ `ads1299_port` file.

Configure the selected TXZ SPI peripheral as master, 8-bit, MSB-first, ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz during bring-up. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input. Keep actual pins model/board-specific.

Recommended runtime flow:

1. Initialize clocks, GPIO and SPI.
2. Build the existing `ads1299_port_t` callbacks.
3. Initialize the ADS1299 core and verify the device ID.
4. Run the internal-test profile.
5. Run the input-short profile.
6. Apply the 250 SPS EEG profile.
7. Start RDATAC streaming.
8. On each DRDY event, service exactly one ADS1299-8 frame (27 bytes).
9. Push the completed frame into a bounded ring buffer for downstream transport.

Keep the DRDY ISR minimal. First prove a blocking transfer with the internal test source, then add DMA/non-blocking transfers where the selected TXZ family supports them. Track SPI errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes only when sufficient resources are exposed by the selected TXZ device. For strict synchronization, prefer `firmware/fpga/common` as the deterministic front end.

Validation order:

`ID -> internal test -> input short -> 250 SPS EEG -> higher rate if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`
