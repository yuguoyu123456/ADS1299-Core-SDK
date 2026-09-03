# Contributing to ADS1299-Core-SDK

ADS1299-Core-SDK is intended to be a conservative, reproducible hardware SDK and learning ecosystem. **Correctness, reproducibility and documentation matter more than the number of folders or lines of code.**

## Core rules

1. Do not duplicate the common ADS1299 driver for each MCU. The shared device layer belongs in `firmware/core_driver/ads1299/`.
2. A visible MCU target belongs under `firmware/mcu/<vendor>/<target>/` and should be a complete reference project. Its platform-specific SPI/GPIO/DRDY adaptation belongs inside that project, not in a separate top-level `ports/` tree.
3. Every platform contribution must state the exact MCU, development board, SDK/compiler version, SPI mode, pin mapping and build procedure.
4. Never mark a platform `Bench-tested` without real ADS1299 hardware evidence.
5. Algorithm examples must state whether they are `Educational`, `Dataset-tested`, `Hardware-tested` or `Online-tested`.
6. Do not copy substantial third-party EEG/BCI code into this repository without license review. Prefer clean implementation, dependencies, interoperability examples and proper attribution.
7. Keep units, data types, sample rates, array dimensions and channel ordering explicit.
8. Avoid unexplained magic numbers when a named constant, enum or readable API can express the same behavior.
9. Treat the current TI ADS1299 documentation as the device-level electrical and protocol authority.
10. CI success is required where a toolchain can reasonably be automated, but CI compilation is not a substitute for real hardware validation.

## What counts as a real platform project

A platform is not considered supported merely because a directory exists. A serious reference project should contain, as applicable:

- complete build files;
- executable `main` / application entry point;
- board clock and peripheral initialization;
- documented pin mapping;
- SPI initialization;
- DRDY handling;
- RESET / START handling;
- a local ADS1299 platform-adaptation layer;
- ADS1299 ID read;
- register read/write path;
- internal test-signal configuration;
- 27-byte frame acquisition;
- signed 24-bit decoding;
- sequence / timestamp handling where relevant;
- host streaming example;
- build, flash and expected-output documentation.

## Minimum platform PR evidence

- ID register read;
- register write/readback;
- 8-channel internal test signal;
- 27-byte frame capture;
- signed 24-bit decoding;
- 250 SPS stream;
- common packet sequence/integrity verification.

Higher sample-rate and long-duration results should be reported separately.

## Repository placement guide

- ADS1299 device code → `firmware/core_driver/ads1299/`
- complete MCU project → `firmware/mcu/<vendor>/<target>/`
- reusable embedded utility → `firmware/common/`
- FPGA RTL / board project → `firmware/fpga/`
- PC receiving / recording / plotting → `pc_tools/`
- EEG signal analysis → `eeg_analysis/`
- BCI paradigm or classifier workflow → `bci/`
- datasets / preparation scripts → `datasets/`
- engineering documentation → `docs/`
- automated tests → `tests/`

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

- datasheet correctness;
- timing and SPI-state correctness;
- reserved-bit handling;
- error propagation;
- buffer safety;
- interrupt behavior;
- dropped-frame detection;
- endianness and sign extension;
- reproducible builds;
- clear user-facing documentation.

## Validation labels

- `Planned` — roadmap only.
- `Compiles` — toolchain build verified.
- `Bench-tested` — real ADS1299-Core hardware tested.
- `24h-tested` — long-run acquisition verified.

Never upgrade a label based on assumption.

## Community standard

Be precise, technical and welcoming. Good bug reports, measurements, logic-analyzer traces, oscilloscope captures, CI logs, reproducible examples and documentation improvements are all valuable contributions.

For contact and collaboration, see [`CONTACT.md`](CONTACT.md).
