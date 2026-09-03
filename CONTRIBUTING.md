# Contributing to ADS1299-Core-SDK

ADS1299-Core-SDK is intended to be a conservative, reproducible hardware SDK and learning ecosystem. **Correctness, reproducibility and documentation matter more than the number of folders or lines of code.**

## Core rules

1. Do not duplicate the common ADS1299 driver for each MCU.
2. Platform-specific low-level code belongs under `ports/`; complete board/toolchain applications belong under `projects/`.
3. Every platform contribution must state the exact MCU, development board, SDK/compiler version, SPI mode, pin mapping and build procedure.
4. Never mark a platform `Bench-tested` without real ADS1299 hardware evidence.
5. Algorithm examples must state whether they are `Educational`, `Dataset-tested`, `Hardware-tested` or `Online-tested`.
6. Do not copy substantial third-party EEG/BCI code into this repository without license review. Prefer clean-room implementation, dependencies, interoperability examples and proper attribution.
7. Keep units, data types, sample rates, array dimensions and channel ordering explicit.
8. Avoid unexplained magic numbers when a named constant, enum or readable API can express the same behavior.
9. Treat the current TI ADS1299 documentation as the device-level electrical and protocol authority.
10. CI success is required where a toolchain can reasonably be automated, but CI compilation is not a substitute for real hardware validation.

## What counts as a real platform project

A platform is not considered supported merely because a directory exists. A serious reference project should contain, as applicable:

- complete build files
- executable `main` / application entry point
- board clock and peripheral initialization
- documented pin mapping
- SPI initialization
- DRDY handling
- RESET / START handling
- ADS1299 ID read
- register read/write path
- internal test-signal configuration
- 27-byte frame acquisition
- signed 24-bit decoding
- sequence / timestamp handling where relevant
- host streaming example
- build, flash and expected-output documentation

## Minimum platform PR evidence

- ID register read
- register write/readback
- 8-channel internal test signal
- 27-byte frame capture
- signed 24-bit decoding
- 250 SPS stream
- host packet sequence/CRC

Higher sample-rate and long-duration results should be reported separately.

## Documentation requirements

Every major folder and complete reference project should eventually contain a local README that answers:

1. What is this folder/project for?
2. Which exact hardware/toolchain does it target?
3. What is implemented today?
4. What is not implemented yet?
5. How do I build it from a clean machine?
6. How do I wire the ADS1299?
7. What output should I expect?
8. What is the current validation level?
9. Where should a beginner go next?

Documentation is part of the product. A technically correct project that a new engineer cannot build or understand is not finished.

## Code review priorities

Reviewers should prioritize:

- datasheet correctness
- timing and SPI-state correctness
- reserved-bit handling
- error propagation
- buffer safety
- interrupt behavior
- dropped-frame detection
- endianness and sign extension
- reproducible builds
- clear user-facing documentation

## Validation labels

- `Planned` — roadmap / placeholder only
- `Compiles` — toolchain build verified
- `Bench-tested` — real ADS1299-Core hardware tested
- `24h-tested` — long-run acquisition verified

Never upgrade a label based on assumption.

## Community standard

Be precise, technical and welcoming. Good bug reports, measurements, logic-analyzer traces, oscilloscope captures, CI logs, reproducible examples and documentation improvements are all valuable contributions.

For contact and collaboration, see [`CONTACT.md`](CONTACT.md).
