# MM32W ADS1299 Ready-to-Run Guide

This file is additive. Existing MM32W source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> ring buffer -> wireless/other transport task`

## MM32W notes

Keep radio/wireless processing completely outside the timing-critical DRDY path. The interrupt should only capture timing/sequence information and signal acquisition work. Use a bounded ring buffer between acquisition and the wireless stack, and expose queue-overflow counters so RF scheduling delays are visible instead of silently dropping EEG frames.

First prove the blocking SPI path with the internal test source. Add DMA only after that path is stable.

For 8 x ADS1299 / 64-channel systems, separate deterministic acquisition from wireless transmission. A dedicated FPGA or higher-capability acquisition controller is preferred for multi-device capture, while MM32W can act as a wireless gateway.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> transport/radio stress -> lead-off/BIAS -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/mindmotion_ads1299_ready_app.[ch]`.
