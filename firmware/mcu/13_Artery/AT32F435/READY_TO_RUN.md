# AT32F435 ADS1299 Ready-to-Run Guide

This file is additive. Existing AT32F435 source, board, port, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1). Start around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge EXINT input.
3. Reuse the existing `ads1299_port` implementation and initialize the portable ADS1299 core.
4. Read the device ID and verify the expected ADS1299 variant.
5. Run the internal-test profile, then the input-short profile.
6. Start normal EEG streaming at 250 SPS before higher data rates are enabled.

## Acquisition path

One ADS1299-8 RDATAC frame is exactly 27 bytes. Service exactly one complete frame per DRDY event.

Recommended architecture:

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte read -> status/header check -> ring buffer -> transport`

Keep USB/UART/Ethernet processing and floating-point conversion outside the timing-critical path.

## AT32F435 notes

AT32F435 is the strongest fit in this Artery set for sustained ADS1299 acquisition. After blocking-transfer validation, use SPI DMA and ping-pong/ring buffers. Track SPI errors, late service, sequence gaps and queue overflows explicitly.

For 8 x ADS1299 / 64 channels, AT32F435 can supervise multiple SPI/DMA lanes, but for tight inter-device skew the FPGA capture layer in `firmware/fpga/common` remains the preferred deterministic front end.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/artery_ads1299_ready_app.[ch]`.
