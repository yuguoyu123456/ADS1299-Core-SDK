# Validation policy

This repository distinguishes source-code existence from actual hardware support.

## MCU/SoC validation

### Planned
Folder/API/roadmap exists. No build claim.

### Compiles
The exact target, SDK/compiler version and build command are recorded and the reference project builds without errors.

### Bench-tested
A real ADS1299 board passes at least:

1. power/clock/reset inspection
2. ID read
3. register read/write
4. all-channel internal test signal
5. 27-byte frame capture
6. sign-extension check
7. 250 SPS continuous stream
8. host packet sequence/CRC check

### 24h-tested
Continuous acquisition runs for at least 24 hours with logged frame-counter/packet-loss results.

## Algorithm validation

- `Educational`: code demonstrates a method on synthetic/example data
- `Dataset-tested`: a named dataset and reproducible result are recorded
- `Hardware-tested`: ADS1299-Core data have been processed successfully
- `Online-tested`: acquisition -> window -> algorithm -> output runs in real time

## Release rule

A README badge or support-table cell must never be upgraded by assumption. Record the board, firmware commit, toolchain, sample rate, gain, clock source, test setup and result.
