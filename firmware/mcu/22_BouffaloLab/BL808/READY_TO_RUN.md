# BL808 ADS1299 Ready-to-Run Guide

This file is additive. Existing BL808 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure the acquisition SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before enabling higher rates.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event.

`DRDY -> high-priority acquisition context -> SPI/DMA 27-byte read -> frame validation -> bounded queue -> other core/network/media/transport`

## BL808 notes

BL808 provides substantially more system capability than a simple MCU, so isolate acquisition from Wi-Fi, BLE, media and application workloads. Where the software architecture permits, dedicate one execution context/core to ADS1299 acquisition and transfer completed frames to other contexts through bounded queues. Do not perform inter-core blocking operations in the DRDY ISR.

First prove the blocking SPI path with the ADS1299 internal test source, then move to DMA/non-blocking transfers. Track sequence gaps, SPI errors, late service, queue overflow and inter-core backpressure.

For 8 x ADS1299 / 64 channels, multiple SPI/DMA lanes may be used when available. For strict low-skew synchronization, use `firmware/fpga/common` as the deterministic capture front end and let BL808 handle buffering, networking and higher-level processing.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> DMA/inter-core stress -> network/media stress -> lead-off/BIAS -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/bouffalo_ads1299_ready_app.[ch]`.
