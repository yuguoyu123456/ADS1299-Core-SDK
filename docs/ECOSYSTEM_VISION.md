# ADS1299-Core-SDK Ecosystem Vision

## Mission

Build the world's most comprehensive open ADS1299 ecosystem for learning, engineering, research and reproducible development.

The repository should allow a newcomer to progress through the complete chain:

**Understand ADS1299 → connect hardware → build firmware → verify internal test signals → acquire real channels → stream data → inspect quality → analyze EEG → build BCI experiments.**

## What “complete” means

A large repository is not automatically a complete ecosystem. This project measures completeness across multiple layers:

1. **Device-level coverage**
   - register definitions and commands;
   - configuration APIs;
   - internal test signal and input-short noise test;
   - channel gain / MUX / SRB;
   - BIAS;
   - lead-off;
   - GPIO;
   - clock and topology options;
   - continuous and command-based data paths;
   - frame decoding and voltage conversion.

2. **Controller coverage**
   - major global MCU ecosystems;
   - representative development boards;
   - complete build systems rather than isolated snippets;
   - reproducible compiler / SDK versions;
   - board pin maps;
   - SPI + DRDY integration;
   - working application entry points;
   - local per-platform adaptation inside each complete project.

3. **FPGA coverage**
   - SPI engine;
   - device-control state machine;
   - frame acquisition;
   - channel unpacking;
   - buffering;
   - board top-levels;
   - constraints;
   - simulation testbenches.

4. **PC tooling**
   - common packet protocol;
   - serial / USB acquisition;
   - integrity checks;
   - timestamps and sequence counters;
   - logging;
   - plotting and inspection.

5. **Signal-processing coverage**
   - Python;
   - MATLAB;
   - R;
   - preprocessing;
   - filtering;
   - PSD and band power;
   - ERP;
   - connectivity;
   - quality control;
   - artifact handling;
   - feature extraction and statistics.

6. **BCI coverage**
   - SSVEP;
   - motor imagery;
   - P300;
   - online windows and buffering;
   - classical machine learning;
   - Riemannian methods;
   - deep-learning examples where justified.

7. **Learning and documentation**
   - beginner path;
   - datasheet interpretation;
   - hardware bring-up;
   - debugging guides;
   - platform-specific quick starts;
   - validation reports;
   - reproducible examples.

8. **Quality and maintenance**
   - CI builds;
   - tests;
   - validation labels;
   - issue templates;
   - contribution rules;
   - versioned releases;
   - changelog and compatibility notes.

## Global positioning

The project is intentionally ambitious. Our target is for ADS1299-Core-SDK to become the **largest and most complete open ADS1299 learning-and-development ecosystem in the world**.

Public claims must remain evidence-based. Until breadth, working-platform count, validation coverage, documentation depth, external contributors, stars, forks, citations and downstream use support the strongest wording, project communication should use phrases such as:

- “Building the world's most comprehensive open ADS1299 ecosystem.”
- “A cross-platform ADS1299 SDK, learning hub, EEG toolkit and BCI ecosystem.”
- “One repository from ADS1299 register access to EEG/BCI workflows.”

When evidence becomes strong enough, the maintainers can update the wording to a direct claim.

## Reader-first repository design

A user should understand the repository within minutes.

Recommended navigation hierarchy:

- **I am completely new:** `getting_started/`
- **I need hardware information:** `hardware/`
- **I use an MCU:** `firmware/mcu/`
- **I use FPGA:** `firmware/fpga/`
- **I need the portable ADS1299 driver:** `firmware/core_driver/ads1299/`
- **I need PC recording / visualization:** `pc_tools/`
- **I want EEG analysis:** `eeg_analysis/`
- **I want BCI:** `bci/`
- **I need datasets:** `datasets/`
- **I need validation / engineering detail:** `docs/`
- **I want to contribute:** `CONTRIBUTING.md`
- **I want to contact the maintainer:** `CONTACT.md`

Every major folder should contain a local README explaining:

- what the folder is for;
- who should use it;
- what to run first;
- dependencies;
- current validation status;
- where to go next.

## Repository architecture principle

The public tree is organized around what a user wants to accomplish.

A user should **not** have to gather one controller project from separate `ports/`, `examples/` and `projects/` trees. The intended model is:

```text
firmware/mcu/<vendor>/<target>/
├── build system
├── application entry point
├── board/BSP code
├── local ADS1299 platform adaptation
├── exact pin map
├── bring-up flow
└── project README
```

The portable ADS1299 programming model remains shared under `firmware/core_driver/ads1299/`.

## Maintenance principles

1. **No fake platform support.** A directory is not a working port.
2. **No meaningless code inflation.** More lines are useful only when they implement real device behavior, portability, checks, examples or documentation.
3. **No hidden magic numbers when a readable API is possible.**
4. **No unsupported hardware claims.** Validation labels describe what was actually tested.
5. **No copied third-party code without license review.** Prefer clean implementation, interoperability and proper attribution.
6. **Every reference project should be understandable by a new engineer.**
7. **Prefer automated builds and tests over confidence by inspection.**
8. **Treat TI documentation as the device-level electrical and protocol authority.**
9. **Keep PC-side APIs independent of the selected MCU whenever possible.**
10. **Documentation is part of the product, not an afterthought.**

## Promotion strategy

The strongest promotion for an engineering repository is reproducible usefulness. Public-facing communication should emphasize:

- real controller projects, not placeholder folders;
- one common ADS1299 driver;
- one common data protocol;
- internal-test-first hardware bring-up;
- standard multi-device SPI with independent chip selects for scalable systems;
- EEG + BCI learning path;
- transparent validation status;
- CI-backed builds;
- beginner-friendly documentation;
- open contribution.

Future launch material should include:

- an English README as the global landing page;
- a Chinese quick-start path for the Chinese developer community;
- architecture and data-flow diagrams;
- support-matrix graphics;
- build screenshots / terminal output;
- internal-test waveform examples;
- input-short noise measurements;
- real EEG acquisition examples after hardware validation;
- 8/16/32/64-channel demonstrations;
- release notes;
- an evidence-based comparison showing the scope covered relative to narrower ADS1299 projects.

## Maintainer contact

Biomedical Engineering / Medical PhD researcher

- **yuguoyu520@gmail.com**
- **yuguoyu123456@126.com**

Collaboration, testing reports, platform contributions, research discussion, education and engineering cooperation are welcome.
