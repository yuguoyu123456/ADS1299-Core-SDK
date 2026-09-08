# CH32V307 ADS1299 Ready-to-Run Guide

This guide is additive. Existing source files remain unchanged.

## Recommended configuration

- ADS1299 SPI: mode 1 (CPOL=0, CPHA=1).
- Start SCLK conservatively around 2-4 MHz during bring-up.
- GPIO outputs: CS, RESET, PWDN, START.
- DRDY: falling-edge interrupt input.
- Use the existing `ads1299_port` implementation and the shared WCH ready application layer.

## Acquisition sequence

1. Initialize the board and SPI/GPIO layer.
2. Read and validate the ADS1299 ID register.
3. Apply the internal-test profile and verify a stable calibration waveform.
4. Apply the input-short profile and verify low/noise-consistent ADC output.
5. Apply the 250 SPS EEG profile and start RDATAC acquisition.
6. On each DRDY event, transfer exactly one complete ADS1299-8 frame (27 bytes).
7. Validate the 0xC status-header nibble and enqueue the frame for downstream transport.

## CH32V307 implementation notes

CH32V307 is a stronger fit than the smallest WCH devices for sustained ADS1299 acquisition. After the blocking path is proven, use SPI DMA and a ping-pong or ring buffer so the CPU can parse or transport the previous frame while the next transfer is serviced.

Keep the DRDY ISR minimal: timestamp/sequence capture plus event signaling only. USB/Ethernet/UART formatting must run outside the timing-critical acquisition path.

For 8 x ADS1299 / 64 channels, use multiple SPI/DMA lanes when practical. For tight inter-device skew and deterministic capture, the FPGA capture layer in `firmware/fpga/common` remains the preferred architecture, with CH32V307 used for control and transport.

## Validation order

`ID -> internal test -> input short -> 250 SPS EEG -> 500/1000 SPS if needed -> lead-off/BIAS -> transport stress -> multi-device sync`

See `../DIRECT_USE.md` and `../_shared/wch_ads1299_ready_app.[ch]`.
