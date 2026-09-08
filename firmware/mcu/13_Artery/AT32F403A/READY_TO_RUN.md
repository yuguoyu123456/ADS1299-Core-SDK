# AT32F403A ADS1299 Ready-to-Run Guide

This file is additive. Existing AT32F403A port, board, example and test files remain unchanged.

## Bring-up

1. Configure AT32 SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge EXINT input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run the internal-test profile, then the input-short profile.
6. Start normal EEG at 250 SPS.

## Continuous frame path

For ADS1299-8, one RDATAC frame is 27 bytes: 3 status bytes plus 8 x 24-bit samples.

Recommended runtime path:

`DRDY falling edge -> acquisition event -> 27-byte SPI read -> status/header validation -> ring buffer -> transport`

Keep the ISR short and keep UART/USB/network formatting outside the timing-critical path.

## AT32F403A notes

AT32F403A is a practical MCU for one-device acquisition and for control/transport roles in larger systems. Bring up a blocking SPI transfer first, then enable DMA after internal-test data is stable. Prefer fixed-size buffers and explicit counters for read errors, sequence gaps, late service and queue overflow.

For 8 x ADS1299 / 64-channel designs, use multiple SPI/DMA lanes where possible. For the tightest synchronization, use the repository FPGA capture layer and let AT32F403A manage configuration and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/artery_ads1299_ready_app.[ch]`.
