# N32L406 ADS1299 Ready-to-Run Guide

This file is additive. Existing N32L406 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event and enqueue it before any transport work.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> fixed ring buffer -> transport`

## N32L406 notes

N32L406 is a low-power-oriented device, so keep acquisition deterministic and let the CPU sleep only when the DRDY service deadline can still be met. Use fixed-size buffers, minimize wake-to-SPI latency and keep transport outside the interrupt. Validate a blocking path first, then add DMA if the selected SDK/peripheral routing supports it.

For 8 x ADS1299 / 64-channel systems, use N32L406 primarily for control/interface duties or use a more capable controller/FPGA for deterministic aggregation.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> low-power wake/sleep stress`.

See `../DIRECT_USE.md` and `../_shared/nationstech_ads1299_ready_app.[ch]`.
