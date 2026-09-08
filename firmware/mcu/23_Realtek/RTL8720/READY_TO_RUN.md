# RTL8720 ADS1299 Ready-to-Run Guide

This file is additive. Existing RTL8720 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the ADS1299 device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI/DMA read -> frame validation -> bounded ring buffer -> Wi-Fi/BLE/UART task`

## RTL8720 notes

Keep Wi-Fi/network scheduling completely outside the timing-critical acquisition path. The DRDY interrupt should only capture timing/sequence information and signal a high-priority acquisition context. Use bounded queues and explicit overflow counters so network stalls cannot silently drop EEG data.

First prove blocking SPI using the internal test source, then enable DMA/non-blocking transfer support if needed.

For 8 x ADS1299 / 64-channel systems, use RTL8720 primarily as a communications/control gateway unless enough deterministic capture resources exist. For strict synchronization, use the repository FPGA capture layer.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> network stress -> lead-off/BIAS -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/realtek_ads1299_ready_app.[ch]`.
