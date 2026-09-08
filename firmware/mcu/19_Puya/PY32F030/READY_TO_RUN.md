# PY32F030 ADS1299 Ready-to-Run Guide

This file is additive. Existing PY32F030 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable core.
4. Read and validate the ADS1299 ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event and enqueue it before transport work.

`DRDY -> acquisition event -> 27-byte SPI read -> frame validation -> fixed ring buffer -> transport`

## PY32F030 notes

PY32F030 is a resource-constrained device. Keep buffers fixed-size, avoid floating-point work in the acquisition path and prefer raw 24-bit sample transport. Validate a blocking SPI implementation first. Do not assume this device is suitable for direct 64-channel aggregation.

For 8 x ADS1299 / 64 channels, use PY32F030 primarily for control/interface duties or pair it with a deterministic FPGA/higher-capability acquisition controller.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> lead-off/BIAS -> transport stress`.

See `../DIRECT_USE.md` and `../_shared/puya_ads1299_ready_app.[ch]`.
