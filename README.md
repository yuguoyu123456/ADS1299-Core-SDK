# ADS1299-Core-SDK

> **Mission: build the world's most comprehensive open ADS1299 ecosystem — from bare-metal MCU/FPGA acquisition to host streaming, EEG analysis and BCI starter workflows.**

A vendor-oriented, cross-platform SDK and learning hub for **Texas Instruments ADS1299** based 8-channel biopotential acquisition systems.

The long-term goal is simple: **if someone anywhere in the world is building with ADS1299, this repository should be one of the first places they come to learn, verify hardware, start firmware, stream data, analyze EEG and build BCI applications.**

> **Status: v0.1 production-SDK buildout / pre-hardware-validation.** A folder existing does **not** mean that platform is hardware-verified. We prefer honest validation labels over inflated platform counts.

## What we are building

ADS1299-Core-SDK is intended to become a complete ADS1299 ecosystem, not merely a single driver file. The repository is organized around the full path from chip to research application:

1. **ADS1299 device driver** — register access, configuration, test signals, BIAS, lead-off, GPIO, frame acquisition and conversion helpers.
2. **Real MCU projects** — complete board/toolchain projects for major global MCU families, with build instructions, pin maps, SPI/DRDY handling and reproducible examples.
3. **FPGA projects** — SPI master, controller, frame reader/unpacker, FIFO, top-level integration, constraints and testbenches.
4. **Common streaming protocol** — one packet format across MCU/SoC/FPGA targets so host tools do not care which controller is underneath.
5. **Host tools** — serial capture, packet decoding, integrity checks, logging and visualization foundations.
6. **EEG algorithms** — Python, MATLAB and R starter workflows for preprocessing, filtering, PSD, band power, ERP, connectivity, QC and feature extraction.
7. **BCI starter kit** — SSVEP, motor imagery, P300 and online-window examples, with a roadmap toward reproducible advanced baselines.
8. **Documentation and learning** — datasheet-grounded explanations, bring-up guides, debugging checklists, validation reports and reference material.

## Our quality promise

We do **not** count a directory as platform support. A controller target should only graduate to a supported reference project when it has a real build system, executable entry point, board configuration, documented pinout, ADS1299 initialization path, internal-test acquisition, continuous streaming and reproducible build instructions.

Validation labels used throughout the project:

- `Planned` — roadmap or placeholder only
- `Compiles` — relevant toolchain build verified
- `Bench-tested` — tested on real ADS1299-Core hardware
- `24h-tested` — long-duration acquisition verified

BCI examples additionally use `Educational`, `Dataset-tested`, `Hardware-tested`, and `Online-tested`.

## Hardware facts used by the SDK

- ADS1299: 8 simultaneous 24-bit biopotential channels
- SPI-compatible digital interface
- 250 SPS to 16 kSPS device data-rate range
- one conversion frame: 24-bit status + 8 × 24-bit channels = 216 bits / 27 bytes
- serial timing uses CPOL=0, CPHA=1

Always treat the **current TI ADS1299 datasheet** as the electrical and timing authority.

## Multi-ADS1299 architecture policy

For 16/32/64-channel systems, the project's primary architecture is **standard/cascaded SPI with an independent `CS` for every ADS1299**, not daisy-chain mode.

The normal topology is:

- shared `SCLK`;
- shared `DIN/MOSI`;
- shared `DOUT/MISO` where the TI cascaded topology is used;
- one dedicated `CS` per ADS1299;
- common clock and synchronized START strategy;
- per-device `DRDY` retained during bring-up whenever GPIO resources permit.

This keeps every converter independently configurable and much easier to debug or isolate. Daisy-chain mode remains an optional ADS1299 feature for special low-pin-count designs, but it is not the default reference architecture for this project.

See [`docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](docs/MULTI_ADS1299_SPI_ARCHITECTURE.md).

## Repository map

```text
ADS1299-Core-SDK/
├── drivers/ads1299/         # portable ADS1299 device driver
├── ports/                   # low-level MCU/SoC portability layers
├── projects/                # complete board/toolchain reference projects
├── examples/                # focused usage examples and demonstrations
├── fpga/                    # reusable RTL, board projects and testbenches
├── host/                    # common packet protocol, capture and tooling
├── algorithms/              # EEG analysis in Python / MATLAB / R
├── bci/                     # SSVEP / motor imagery / P300 / online BCI
├── docs/                    # bring-up, support matrix, validation, references
├── tests/                   # C / Python / RTL checks
├── CONTRIBUTING.md          # contribution and quality rules
└── CONTACT.md               # maintainer contact information
```

## Global controller roadmap

The platform roadmap covers:

- **STMicroelectronics:** STM32 families
- **GigaDevice:** GD32 families
- **WCH:** CH32 families
- **Espressif:** ESP32 families
- **Nordic:** nRF52 / nRF53
- **Raspberry Pi:** RP2040 / RP2350
- **NXP:** MCX / i.MX RT class targets
- **Microchip:** 32-bit MCU families
- **Renesas:** RA families
- **Texas Instruments:** MCU targets
- **Infineon / Cypress:** PSoC-class targets
- **Silicon Labs:** EFR32-class targets
- **Chinese MCU ecosystems:** HC32, AT32, MM32, N32, PY32, APM32 and others
- **8-bit ecosystems:** AVR and STC where technically appropriate
- **FPGA:** AMD/Xilinx, Intel/Altera and Lattice tracks

See [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md) for the actual validation state. **Roadmap coverage is not the same as verified support.**

## EEG analysis

Host-side starter algorithms are provided/planned in:

- **Python** — NumPy/SciPy first; interoperability with MNE-Python, scikit-learn, pyRiemann and MOABB
- **MATLAB** — transparent starter functions; interoperability with EEGLAB and FieldTrip
- **R** — readable starter analysis and reproducible statistical workflows

Methods include preprocessing, rereferencing, notch/band-pass filtering, PSD, band power, alpha peak, Hjorth parameters, ERP, connectivity and artifact/QC workflows.

## BCI

Educational and engineering baselines include:

- **SSVEP:** canonical correlation analysis (CCA)
- **Motor imagery:** common spatial patterns (CSP)
- **P300:** baseline correction, ERP averaging, amplitude and peak latency
- **Online:** ring buffer and sliding-window pipeline foundations

Roadmap: FBCCA, harmonic SNR, ERD/ERS, FBCSP, LDA/SVM, Riemannian features, xDAWN, EEGNet and reproducible online demonstrations.

## First hardware acceptance sequence

1. Verify power rails, reference, clock and reset electrically.
2. Read the ADS1299 ID register.
3. Read/write registers after leaving continuous-read mode.
4. Capture the internal test signal on all eight channels.
5. Verify correct 24-bit two's-complement decoding.
6. Verify 250 SPS first, then higher configured rates.
7. Stream the common host packet and verify sequence/CRC on the host.
8. Run long-duration acquisition before declaring the platform hardware-supported.

## For contributors

We welcome firmware engineers, FPGA developers, neuroscientists, biomedical engineers, signal-processing researchers, BCI developers, educators and students.

Please read [`CONTRIBUTING.md`](CONTRIBUTING.md). The guiding principle is:

> **Correctness, reproducibility and clear documentation matter more than the number of folders.**

If you contribute a new platform, make it a real project that another person can clone, build, flash and understand.

## Project positioning

Our ambition is to make ADS1299-Core-SDK the **largest and most complete open ADS1299 learning-and-development ecosystem in the world**. Until that claim is objectively demonstrated by breadth, working examples, validation coverage and community adoption, we deliberately phrase it as our **mission**, not as an unverified fact.

That distinction is important: the project should become famous because it is genuinely useful, technically trustworthy and easy to learn from — not because the README makes a claim we cannot yet prove.

## Contact

Project maintainer: **Biomedical Engineering / Medical PhD researcher**

For technical discussion, collaboration, research exchange, hardware feedback, bug reports, teaching use or contributions, you are very welcome to contact me:

- **Gmail:** yuguoyu520@gmail.com
- **Email:** yuguoyu123456@126.com

Community issues and pull requests are strongly encouraged so that solutions can benefit everyone.

## Intended use

Research, education, prototyping and engineering development. This repository is **not** a medical device, diagnostic system or validated clinical software.
