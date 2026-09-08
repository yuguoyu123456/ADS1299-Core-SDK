# HC32F4A0 ADS1299 Ready-to-Run Guide

This file is additive. Existing HC32F4A0 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read the ADS1299 device ID and validate the expected variant.
5. Run the internal-test profile and verify a stable calibration waveform.
6. Run the input-short profile.
7. Start normal EEG acquisition at 250 SPS before higher rates are enabled.

## Acquisition path

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> frame validation -> ring buffer -> transport`

Keep logging, packetization and floating-point conversion outside the timing-critical ISR.

## HC32F4A0 notes

HC32F4A0 is well suited to sustained ADS1299 acquisition. First prove the blocking path, then use SPI DMA with ping-pong or ring buffering. Expose counters for SPI errors, sequence gaps, late service and buffer overflow.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes where practical. For strict synchronization and low inter-device skew, the repository FPGA capture layer remains the preferred deterministic front end.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/hdsc_ads1299_ready_app.[ch]`.
