# TXZ ADS1299 Ready-to-Run Guide

This file is additive. Existing TXZ source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before raising the data rate.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> acquisition event -> SPI/DMA 27-byte read -> status/header validation -> bounded ring buffer -> transport`

## TXZ notes

TXZ covers multiple device families, so keep actual pin maps, SPI instances and DMA routing model-specific. Prove the blocking transfer first using the ADS1299 internal test source, then add DMA/non-blocking transfers where supported. Keep logging, packetization and floating-point conversion outside the DRDY ISR.

For 8 x ADS1299 / 64-channel systems, multiple SPI/DMA lanes can be used when the selected TXZ device has sufficient resources. For strict low-skew synchronization, use the repository FPGA capture layer as the deterministic front end.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> higher rate if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/toshiba_ads1299_ready_app.[ch]`.
