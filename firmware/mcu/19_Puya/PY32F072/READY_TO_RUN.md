# PY32F072 ADS1299 Ready-to-Run Guide

This file is additive. Existing PY32F072 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI read -> status/header validation -> ring buffer -> transport`

## PY32F072 notes

PY32F072 is suitable for a compact single-ADS1299 node at conservative rates. Keep memory use deterministic and transport outside the ISR. First validate a blocking SPI transfer with the internal test source; add DMA only if the selected SDK/peripheral mapping supports it and only after the blocking path is stable.

For 8 x ADS1299 / 64-channel systems, use a more capable MCU/FPGA as the main deterministic aggregator.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`.

See `../DIRECT_USE.md` and `../_shared/puya_ads1299_ready_app.[ch]`.
