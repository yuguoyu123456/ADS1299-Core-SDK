# N32G455 ADS1299 Ready-to-Run Guide

This file is additive. Existing N32G455 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run the internal-test profile and then the input-short profile.
6. Start normal EEG acquisition at 250 SPS before enabling higher rates.

For ADS1299-8, one RDATAC frame is 27 bytes. Service one full frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> status/header validation -> ring buffer -> transport`

## N32G455 notes

N32G455 is a strong fit for sustained ADS1299 acquisition. First prove the blocking transfer using the internal test signal; then move to SPI DMA with ping-pong or ring buffering. Keep transport and floating-point conversion outside the timing-critical path and track SPI errors, sequence gaps, late service and queue overflows.

For 8 x ADS1299 / 64 channels, multiple SPI/DMA lanes are preferred. For strict synchronization, use the FPGA capture layer in `firmware/fpga/common` and let N32G455 handle configuration and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/nationstech_ads1299_ready_app.[ch]`.
