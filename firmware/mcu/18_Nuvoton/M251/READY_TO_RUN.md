# M251 ADS1299 Ready-to-Run Guide

This file is additive. Existing M251 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run the internal-test profile and then the input-short profile.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI read -> status/header validation -> ring buffer -> transport`

## M251 notes

M251 is suitable for a compact low-power single-ADS1299 node. Keep wake-to-service latency bounded, use fixed-size buffers and keep transport outside the timing-critical ISR. Validate blocking SPI first; add DMA only after the internal-test signal is stable.

For 8 x ADS1299 / 64-channel systems, use M251 primarily as a control/interface MCU or gateway rather than as the main deterministic aggregator.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> low-power wake/sleep stress`.

See `../DIRECT_USE.md` and `../_shared/nuvoton_ads1299_ready_app.[ch]`.
