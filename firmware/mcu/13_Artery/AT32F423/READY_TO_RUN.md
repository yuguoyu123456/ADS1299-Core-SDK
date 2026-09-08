# AT32F423 ADS1299 Ready-to-Run Guide

This file is additive. Existing AT32F423 source, port, board, example and test files are preserved.

## Bring-up

1. Configure the AT32 SPI peripheral as master, ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as GPIO outputs; configure DRDY as a falling-edge EXINT input.
3. Use the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles before normal electrode acquisition.
6. Begin EEG streaming at 250 SPS and increase the rate only after the entire acquisition/transport chain is stable.

## Acquisition path

An ADS1299-8 RDATAC frame is 27 bytes: 3 status bytes plus 8 x 24-bit channel values.

Use:

`DRDY falling edge -> high-priority acquisition event -> 27-byte SPI transfer -> header/status validation -> ring buffer -> transport`

The interrupt should only capture timing/sequence information and signal the acquisition context.

## AT32F423 notes

AT32F423 is suitable for a deterministic single-ADS1299 node and can use DMA after the blocking path is validated. Use fixed-size frame buffers, explicit overrun counters and separate transport processing from acquisition.

For 8 x ADS1299 / 64-channel designs, prefer multiple SPI/DMA lanes or the repository FPGA capture layer when low inter-device skew is required.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if needed -> lead-off/BIAS -> transport stress -> multi-device sync`

See `../DIRECT_USE.md` and `../_shared/artery_ads1299_ready_app.[ch]`.
