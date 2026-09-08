# CH32V317 ADS1299 Ready-to-Run Guide

This file is additive. No existing WCH port, board, example, test or documentation file is removed or replaced.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1).
2. Use a conservative 2-4 MHz SPI clock first.
3. Configure CS, RESET, PWDN and START as outputs; configure DRDY as a falling-edge interrupt input.
4. Bind the existing `ads1299_port` callbacks and initialize the portable core.
5. Read and validate the ADS1299 ID register.
6. Run internal-test and input-short profiles before normal electrodes are connected.
7. Start normal acquisition at 250 SPS and increase only after the complete data path is stable.

## Frame handling

An ADS1299-8 RDATAC frame is exactly 27 bytes: 3 status bytes plus 8 x 24-bit channel samples. Service one full frame per DRDY event.

Recommended flow:

`DRDY falling edge -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte read -> status/header validation -> ring buffer -> transport`

Do not perform printf, USB packet construction, network work or floating-point conversion in the DRDY ISR.

## CH32V317 notes

CH32V317 is appropriate for a higher-throughput WCH implementation. After the basic blocking transfer is validated, prefer SPI DMA with ping-pong or ring buffering. Keep acquisition higher priority than USB/Ethernet/UART processing and expose counters for SPI errors, sequence gaps, late service and buffer overflow.

For 8 x ADS1299 / 64-channel systems, use independent CS control and multiple SPI/DMA lanes when timing allows. If tight inter-device phase alignment is required, use the repository FPGA capture layer for deterministic acquisition and let CH32V317 handle configuration and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS as required -> lead-off/BIAS -> transport stress -> multi-device synchronization`

See `../DIRECT_USE.md` and `../_shared/wch_ads1299_ready_app.[ch]`.
