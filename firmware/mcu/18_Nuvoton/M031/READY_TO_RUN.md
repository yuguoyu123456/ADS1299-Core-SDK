# M031 ADS1299 Ready-to-Run Guide

This file is additive. Existing M031 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event and enqueue it before transport formatting.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> fixed ring buffer -> transport`

## M031 notes

M031 is best suited to a compact single-ADS1299 node at conservative rates. Use fixed-size buffers and keep memory use deterministic. Validate the blocking path first; add DMA only after the internal-test waveform is stable and only if the selected peripheral mapping supports it.

For 8 x ADS1299 / 64-channel systems, use M031 mainly for control/interface duties rather than direct aggregation.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`.

See `../DIRECT_USE.md` and `../_shared/nuvoton_ads1299_ready_app.[ch]`.
