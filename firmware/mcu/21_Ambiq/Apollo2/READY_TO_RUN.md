# Apollo2 ADS1299 Ready-to-Run Guide

This file is additive. Existing Apollo2 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure IOM/SPI as ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event and enqueue it before transport processing.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> fixed ring buffer -> transport`

## Apollo2 notes

Apollo2 is best suited to a low-power single-ADS1299 node at conservative rates. Keep wake-to-service latency bounded, use fixed-size buffers and keep BLE/UART/logging outside the timing-critical ISR. Validate a blocking transfer first; add DMA only after internal-test data is stable.

For 8 x ADS1299 / 64 channels, use Apollo2 primarily as a low-power control/interface node or gateway rather than the main deterministic aggregator.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> low-power wake/sleep stress -> lead-off/BIAS -> transport stress`.

See `../DIRECT_USE.md` and `../_shared/ambiq_ads1299_ready_app.[ch]`.
