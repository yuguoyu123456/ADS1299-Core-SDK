# N32G430 ADS1299 Ready-to-Run Guide

This file is additive. Existing N32G430 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> ring buffer -> transport`

## N32G430 notes

N32G430 is suitable for a deterministic single-ADS1299 node. First validate blocking SPI with the internal test signal; then add DMA and ping-pong/ring buffering if required. Track read errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64-channel systems, use multiple SPI/DMA lanes where resources allow or offload strict multi-device timing to the repository FPGA capture layer.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if required -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/nationstech_ads1299_ready_app.[ch]`.
