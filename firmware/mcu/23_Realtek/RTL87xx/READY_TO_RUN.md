# RTL87xx ADS1299 Ready-to-Run Guide

This file is additive. Existing RTL87xx source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure the selected SPI peripheral for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before higher data rates are enabled.

For ADS1299-8, one RDATAC frame is exactly 27 bytes. Service one complete frame per DRDY event.

`DRDY -> high-priority acquisition context -> SPI/DMA 27-byte read -> frame validation -> bounded queue -> radio/network/application task`

## RTL87xx notes

RTL87xx covers multiple devices, so keep pin assignment, SPI instance and DMA routing model-specific. Isolate radio/network stacks from acquisition and expose sequence-gap, SPI-error, late-service and queue-overflow counters. First prove the blocking transfer with the ADS1299 internal test source, then move to non-blocking/DMA transfer when supported.

For 8 x ADS1299 / 64-channel systems, use multiple independent capture lanes only when the selected RTL87xx device exposes enough deterministic resources. Otherwise use `firmware/fpga/common` as the acquisition front end and use RTL87xx for communication and control.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> higher rate if required -> radio/network stress -> lead-off/BIAS -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/realtek_ads1299_ready_app.[ch]`.
