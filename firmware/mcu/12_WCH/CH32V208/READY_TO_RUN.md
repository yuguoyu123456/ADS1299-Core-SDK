# CH32V208 ADS1299 Ready-to-Run Guide

This file is additive and does not replace any existing port, board, example, test or documentation file.

## Bring-up sequence

1. Configure SPI master for ADS1299 mode 1 (CPOL=0, CPHA=1), initially around 2-4 MHz.
2. Configure CS, RESET, PWDN and START as outputs and DRDY as a falling-edge interrupt input.
3. Use the existing `ads1299_port` callbacks to create the portable ADS1299 port object.
4. Read the device ID and verify the ADS1299 family/channel count.
5. Validate the digital path with the internal test source.
6. Validate ADC/PGA noise with the input-short profile.
7. Start normal EEG at 250 SPS before raising the data rate.

## Continuous acquisition

For ADS1299-8, one RDATAC frame is exactly 27 bytes: 3 status bytes and 24 channel-data bytes. Do not perform logging, USB work or heavy parsing inside the DRDY ISR.

Recommended path:

`DRDY IRQ -> acquisition event -> 27-byte SPI transfer -> header/status check -> ring buffer -> transport task`

Maintain counters for frame reads, SPI errors, sequence gaps and ring-buffer overflow.

## CH32V208 notes

CH32V208 has enough peripheral capability for a clean one-device acquisition design. Bring up the blocking path first; once the internal-test waveform is stable, DMA can be introduced to reduce CPU jitter. Keep transport work decoupled from the DRDY service path.

For the repository's 8 x ADS1299 / 64-channel target, do not serialize all real-time work through one long ISR. Prefer parallel SPI/DMA resources or the FPGA capture layer for deterministic multi-device timing.

## Validation order

`ID -> internal test -> input short -> EEG 250 SPS -> lead-off/BIAS -> higher data rate -> transport stress -> multi-device sync`

See `../DIRECT_USE.md` and `../_shared/wch_ads1299_ready_app.[ch]`.
