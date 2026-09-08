# Apollo4 ADS1299 Ready-to-Run Guide

This file is additive. Existing Apollo4 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure IOM/SPI for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before raising the sample rate.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> IOM/SPI DMA 27-byte read -> status/header validation -> ring buffer -> transport/processing`

## Apollo4 notes

Apollo4 has enough performance for sustained ADS1299 acquisition plus local processing. First prove the blocking IOM/SPI path using the internal test source, then use DMA/non-blocking transfers with bounded ping-pong or ring buffers. Keep BLE, graphics, DSP and logging outside the real-time acquisition path.

If cache or multiple memory regions are enabled in the selected Apollo4 SDK configuration, ensure DMA buffers are placed in compatible memory and observe any required coherency rules.

For 8 x ADS1299 / 64 channels, use multiple capture lanes when practical. For strict low-skew synchronization, use `firmware/fpga/common` as the deterministic front end and let Apollo4 handle buffering, processing and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> DMA/memory stress -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/ambiq_ads1299_ready_app.[ch]`.
