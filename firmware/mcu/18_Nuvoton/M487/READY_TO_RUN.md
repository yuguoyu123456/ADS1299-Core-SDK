# M487 ADS1299 Ready-to-Run Guide

This file is additive. Existing M487 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before enabling higher rates.

For ADS1299-8, one RDATAC frame is 27 bytes. Service one full frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> frame validation -> ring buffer -> transport`

## M487 notes

M487 is a strong fit for sustained ADS1299 acquisition. First validate blocking SPI with the internal test source; then use SPI DMA and ping-pong/ring buffering. Keep USB/Ethernet/UART processing and floating-point conversion outside the timing-critical acquisition path. Track SPI errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64 channels, multiple SPI/DMA lanes can be supervised by this class of MCU, but for strict low-skew synchronization use `firmware/fpga/common` as the deterministic capture front end.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/nuvoton_ads1299_ready_app.[ch]`.
