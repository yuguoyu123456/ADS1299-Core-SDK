# ADS1299-Core-SDK Ecosystem Vision

## Mission

Build the world's most comprehensive open ADS1299 ecosystem for learning, engineering, research and reproducible development.

The repository should eventually allow a newcomer to progress through the complete chain:

**Understand ADS1299 → connect hardware → build firmware → verify internal test signals → acquire real channels → stream data → inspect quality → analyze EEG → build BCI experiments.**

## What “complete” means

A large repository is not automatically a complete ecosystem. This project measures completeness across multiple layers:

1. **Device-level coverage**
   - register definitions
   - configuration APIs
   - internal test signal
   - channel gain and MUX
   - BIAS
   - lead-off
   - GPIO
   - clock and daisy-chain options
   - continuous and command-based data paths
   - frame decoding and voltage conversion

2. **Controller coverage**
   - major global MCU ecosystems
   - representative development boards
   - complete build systems
   - reproducible compiler / SDK versions
   - board pin maps
   - SPI + DRDY integration
   - working application entry points

3. **FPGA coverage**
   - SPI engine
   - device control state machine
   - frame acquisition
   - channel unpacking
   - buffering
   - board top-levels
   - constraints
   - simulation testbenches

4. **Host tooling**
   - common packet protocol
   - serial acquisition
   - integrity checks
   - timestamps and sequence counters
   - logging
   - plotting and inspection

5. **Signal-processing coverage**
   - Python
   - MATLAB
   - R
   - preprocessing
   - filtering
   - PSD and band power
   - ERP
   - connectivity
   - quality control
   - artifact handling

6. **BCI coverage**
   - SSVEP
   - motor imagery
   - P300
   - online windows
   - classical machine learning
   - Riemannian methods
   - deep-learning examples where justified

7. **Learning and documentation**
   - beginner path
   - datasheet interpretation
   - hardware bring-up
   - debugging guides
   - platform-specific quick starts
   - validation reports
   - reproducible examples

8. **Quality and maintenance**
   - CI builds
   - tests
   - validation labels
   - issue templates
   - contribution rules
   - versioned releases
   - changelog
   - compatibility notes

## Global positioning

The project is intentionally ambitious. Our target is for ADS1299-Core-SDK to become the **largest and most complete open ADS1299 learning-and-development ecosystem in the world**.

However, public claims must remain evidence-based. Until breadth, working-platform count, validation coverage, documentation depth, external contributors, stars, forks, citations and downstream use support the strongest wording, project communication should use phrases such as:

- “Building the world's most comprehensive open ADS1299 ecosystem.”
- “A cross-platform ADS1299 SDK, learning hub, EEG toolkit and BCI starter ecosystem.”
- “One repository from ADS1299 register access to EEG/BCI workflows.”

When the evidence becomes strong enough, the maintainers can update the wording to a direct claim.

## Reader-first repository design

A user should understand the repository within minutes.

Recommended navigation hierarchy:

- **Start here:** root README
- **What hardware/controllers are supported?** `docs/SUPPORT_MATRIX.md`
- **How do I connect my board?** platform project README
- **How does the ADS1299 driver work?** `drivers/ads1299/`
- **How do I stream data?** `host/`
- **How do I analyze EEG?** `algorithms/`
- **How do I start BCI?** `bci/`
- **How do I contribute?** `CONTRIBUTING.md`
- **How do I contact the maintainer?** `CONTACT.md`

Every major folder should eventually contain a local README explaining:

- purpose
- supported contents
- first example to run
- dependencies
- validation status
- links to deeper documentation

## Maintenance principles

1. **No fake platform support.** A folder is not a working port.
2. **No meaningless code inflation.** More lines are useful only when they implement real device behavior, portability, checks, examples or documentation.
3. **No hidden magic numbers when a readable API is possible.**
4. **No unsupported hardware claims.** Validation labels must describe what was actually tested.
5. **No copied third-party code without license review.** Prefer clean implementation, interoperability and proper attribution.
6. **Every reference project should be understandable by a new engineer.**
7. **Prefer automated builds and tests over confidence by inspection.**
8. **Treat TI documentation as the device-level electrical and protocol authority.**
9. **Keep host-side APIs independent of the selected MCU whenever possible.**
10. **Documentation is part of the product, not an afterthought.**

## Promotion strategy

The strongest promotion for an engineering repository is reproducible usefulness. Public-facing communication should consistently emphasize:

- many real controller projects, not placeholder folders
- one common ADS1299 driver
- one common host protocol
- internal-test-first bring-up
- EEG + BCI learning path
- transparent validation status
- CI-backed builds
- beginner-friendly documentation
- open contribution

Future launch material should include:

- an English README as the global landing page
- a Chinese README for the Chinese developer community
- architecture diagram
- support-matrix graphic
- quick-start screenshots / terminal output
- internal test waveform examples
- real EEG acquisition examples after hardware validation
- release notes
- comparison table showing what this repository covers relative to narrower ADS1299 projects

## Maintainer contact

Biomedical Engineering / Medical PhD researcher

- **yuguoyu520@gmail.com**
- **yuguoyu123456@126.com**

Collaboration, testing reports, platform contributions and research discussion are welcome.
