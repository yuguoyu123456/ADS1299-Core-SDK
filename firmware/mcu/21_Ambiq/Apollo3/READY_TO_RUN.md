# Apollo3 ADS1299 Ready-to-Run Guide

This file is additive. Existing Apollo3 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure IOM/SPI for ADS1299 mode 1 (CPOL=0, CPHA=1), beginning around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, service exactly one 27-byte RDATAC frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> IOM/SPI transfer -> frame validation -> ring buffer -> transport`

## Apollo3 notes

Keep low-power management and BLE/transport work outside the timing-critical acquisition path. First validate blocking IOM/SPI with the internal test source. Then use DMA or non-blocking peripheral support where appropriate, keeping bounded buffers and explicit overrun counters.

For 8 x ADS1299 / 64 channels, use Apollo3 primarily as a low-power gateway/control MCU unless enough independent capture resources are available. For strict synchronization, use the repository FPGA capture layer.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> low-power stress -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/ambiq_ads1299_ready_app.[ch]`.
