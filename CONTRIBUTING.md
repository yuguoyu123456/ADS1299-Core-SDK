# Contributing

ADS1299-Core-SDK is intended to be a conservative hardware SDK: correctness and reproducibility matter more than the number of folders.

## Rules

1. Do not duplicate the common ADS1299 driver for each MCU.
2. Platform-specific code belongs under `ports/` and `examples/`.
3. Every platform PR must state the exact MCU/board, SDK/compiler version and SPI configuration.
4. Never mark a platform `Bench-tested` without real ADS1299 hardware evidence.
5. Algorithm examples must state whether they are Educational, Dataset-tested, Hardware-tested or Online-tested.
6. Do not copy substantial third-party EEG/BCI code into this repository. Prefer dependencies and interoperability examples, respecting upstream licenses.
7. Keep units and array dimensions explicit.

## Minimum platform PR evidence

- ID register read
- register write/readback
- 8-channel internal test signal
- 27-byte frame capture
- signed 24-bit decoding
- 250 SPS stream
- host packet sequence/CRC

Higher sample-rate and long-duration results should be reported separately.
