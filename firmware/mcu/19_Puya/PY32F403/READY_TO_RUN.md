# PY32F403 ADS1299 Ready-to-Run Guide

This file is additive. Existing PY32F403 source, port, board, examples and tests remain unchanged.

## Bring-up

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), starting around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs; DRDY as a falling-edge interrupt input.
3. Reuse the existing `ads1299_port` callbacks and initialize the portable ADS1299 core.
4. Read and validate the device ID.
5. Run internal-test and input-short profiles.
6. Start normal EEG acquisition at 250 SPS before higher rates are enabled.

For ADS1299-8, each RDATAC frame is exactly 27 bytes. Service one full frame per DRDY event.

`DRDY -> timestamp/sequence -> acquisition event -> SPI/DMA 27-byte transfer -> frame validation -> ring buffer -> transport`

## PY32F403 notes

PY32F403 is the strongest fit in this Puya set for sustained ADS1299 acquisition. First prove the blocking transfer with the internal test signal, then use DMA and bounded ping-pong/ring buffers where supported. Keep transport and floating-point conversion outside the timing-critical path and track SPI errors, sequence gaps, late servicing and queue overflow.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes when practical. For strict low-skew synchronization, use `firmware/fpga/common` for deterministic capture and let PY32F403 handle control and transport.

Validation order: `ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS -> lead-off/BIAS -> transport stress -> multi-device synchronization`.

See `../DIRECT_USE.md` and `../_shared/puya_ads1299_ready_app.[ch]`.
