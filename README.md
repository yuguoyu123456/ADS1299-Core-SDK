# ADS1299-Core-SDK

> ## Building the world's most comprehensive open ADS1299 ecosystem
> **From register-level control to real MCU/FPGA projects, multi-device acquisition, host streaming, Python/MATLAB/R EEG analysis, and BCI workflows — in one repository.**

**ADS1299-Core-SDK** is a vendor-oriented, cross-platform engineering and learning ecosystem for the **Texas Instruments ADS1299** family of biopotential front ends.

The goal is not to create another ADS1299 code snippet.

The goal is to build a place where a student, researcher, embedded engineer, FPGA developer, neuroscience laboratory, BCI team, medical-device R&D group, or hardware company can start with an ADS1299 board and continue all the way to **validated acquisition, signal analysis and BCI development** without rebuilding the same infrastructure from zero.

> **Project status:** active production-SDK buildout / pre-hardware-validation.  
> A folder is **not** counted as support until it reaches the validation level stated in the support matrix.

---

## Why this repository is different

Most ADS1299 projects focus on only one layer: a driver, one MCU, one board, one application, or one analysis script.

**ADS1299-Core-SDK is being designed as a full-stack ADS1299 ecosystem.**

| Layer | What this repository aims to provide |
|---|---|
| **ADS1299 device layer** | Complete register definitions, commands, channel configuration, BIAS, lead-off, GPIO, clocking, internal test, frame decoding and diagnostics |
| **MCU layer** | Complete reference projects for major global MCU ecosystems rather than isolated snippets |
| **FPGA layer** | SPI engine, device-control FSM, frame acquisition, unpacking, buffering, top-level projects, constraints and testbenches |
| **Multi-device layer** | 8/16/24/32/64-channel scaling with standard multi-device SPI and independent chip-select control |
| **Host layer** | One common packet protocol, capture, integrity checking, recording, visualization and device-independent host tools |
| **EEG layer** | Python, MATLAB and R workflows for preprocessing, filtering, PSD, band power, ERP, connectivity, QC and feature extraction |
| **BCI layer** | SSVEP, motor imagery, P300, online-window pipelines, classical ML, Riemannian methods and future deep-learning baselines |
| **Learning layer** | Datasheet explanations, bring-up guides, debugging checklists, validation reports and beginner-to-research learning paths |

### Our long-term ambition

We are building ADS1299-Core-SDK to become the **largest and most complete open ADS1299 learning-and-development ecosystem in the world**.

That is an engineering target, not an excuse for exaggerated support claims. We would rather have ten reproducible projects than one hundred placeholder folders.

The standard is simple:

> **Clone it. Build it. Flash it. Connect ADS1299. Verify the internal test signal. Stream data. Analyze EEG. Continue into BCI.**

If a project cannot eventually support that workflow, it is not finished.

---

## A chip-to-BCI ecosystem

The distinctive scope of this repository is the complete path from **ADS1299 silicon to BCI research**:

```text
ADS1299
  │
  ├── Register / command driver
  │
  ├── STM32 / GD32 / CH32 / ESP32 / nRF / RP2040 / NXP / TI / ...
  │
  ├── FPGA acquisition
  │
  ├── 8 / 16 / 32 / 64-channel systems
  │
  ├── Common streaming protocol
  │
  ├── Python / MATLAB / R
  │
  ├── EEG preprocessing and analysis
  │
  └── BCI
       ├── SSVEP
       ├── Motor Imagery
       ├── P300
       ├── Online pipelines
       ├── Classical ML
       ├── Riemannian methods
       └── Deep-learning baselines
```

For BCI developers, the intention is that this repository should not stop at “we can read SPI.” It should help bridge **hardware acquisition → trustworthy data → reproducible signal processing → usable BCI baselines**.

---

## Start here

| I want to... | Start with |
|---|---|
| Understand the repository | This README |
| See what controllers are actually supported | [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md) |
| Bring up a new ADS1299 board | [`docs/BRINGUP.md`](docs/BRINGUP.md) |
| Understand 16/32/64-channel scaling | [`docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](docs/MULTI_ADS1299_SPI_ARCHITECTURE.md) |
| Use the portable ADS1299 driver | [`drivers/ads1299/`](drivers/ads1299/) |
| Build a complete MCU example | [`projects/`](projects/) |
| Work with FPGA | [`fpga/`](fpga/) |
| Stream data to a computer | [`host/`](host/) |
| Learn EEG analysis | [`algorithms/`](algorithms/) |
| Start BCI experiments | [`bci/`](bci/) and [`docs/BCI_QUICKSTART.md`](docs/BCI_QUICKSTART.md) |
| Check algorithm coverage | [`docs/ALGORITHM_MATRIX.md`](docs/ALGORITHM_MATRIX.md) |
| Understand project quality rules | [`docs/PROJECT_QUALITY_STANDARD.md`](docs/PROJECT_QUALITY_STANDARD.md) |
| Contribute | [`CONTRIBUTING.md`](CONTRIBUTING.md) |
| Contact the maintainer | [`CONTACT.md`](CONTACT.md) |

---

## Repository map

```text
ADS1299-Core-SDK/
├── drivers/ads1299/         # portable ADS1299 device driver
├── ports/                   # low-level MCU/SoC portability layers
├── projects/                # complete board + toolchain reference projects
├── examples/                # focused demonstrations and usage examples
├── fpga/                    # reusable RTL, board projects and testbenches
├── host/                    # protocol, capture, recording and visualization
├── algorithms/              # EEG analysis in Python / MATLAB / R
├── bci/                     # SSVEP / MI / P300 / online BCI
├── docs/                    # bring-up, architecture, support, validation
├── tests/                   # C / Python / RTL tests
├── CONTRIBUTING.md
└── CONTACT.md
```

Every major folder is expected to become self-explanatory. A new user should be able to understand **what it contains, why it exists, what to run first, which dependencies are required and what has actually been validated**.

---

## Global controller ecosystem

The roadmap intentionally spans both international and Chinese MCU ecosystems.

### Major MCU / SoC families

- **STMicroelectronics** — STM32
- **GigaDevice** — GD32
- **WCH** — CH32
- **Espressif** — ESP32 / ESP32-S3 and related families
- **Nordic Semiconductor** — nRF52 / nRF53
- **Raspberry Pi** — RP2040 / RP2350
- **NXP** — MCX / i.MX RT class targets
- **Microchip** — SAM / PIC32 class targets
- **Renesas** — RA / related families
- **Texas Instruments** — MSPM0 / other suitable MCU targets
- **Infineon / Cypress** — PSoC / XMC class targets
- **Silicon Labs** — EFM32 / EFR32 class targets
- **HDSC** — HC32
- **Artery** — AT32
- **MindMotion** — MM32
- **Nationstech** — N32
- **Puya** — PY32
- **Geehy** — APM32
- **STC** — STC8 / STC32 where technically appropriate
- **Microchip AVR** — selected 8-bit educational/reference targets

### FPGA tracks

- **AMD / Xilinx**
- **Intel / Altera**
- **Lattice**

The repository uses explicit validation labels. See [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md) before assuming that a roadmap target has already been built or bench-tested.

---

## ADS1299 device coverage

The common driver is intended to expose readable APIs rather than forcing users to scatter register “magic numbers” throughout applications.

Coverage includes or is being built for:

- device identification and register dump
- RESET / START / STOP / RDATAC / SDATAC / RDATA
- single and multi-register access
- data-rate configuration
- channel gain / MUX / power state
- SRB1 / SRB2
- BIAS configuration and status
- lead-off configuration and status
- internal test signal
- input-short noise testing
- GPIO
- clock-output control
- standard multi-device operation
- continuous 27-byte frame capture
- signed 24-bit decoding
- ADC-code-to-voltage conversion
- diagnostics and validation helpers

The **current TI ADS1299 datasheet remains the electrical and timing authority**.

---

## Multi-ADS1299 architecture: our default is standard SPI, not daisy-chain

For 16/32/64-channel systems, this project uses **standard/cascaded multi-device SPI with one independent `CS` per ADS1299** as the primary architecture.

Typical topology:

```text
                         +----------- ADS1299 #1  -> CH1-8
MCU / FPGA SCLK --------+----------- ADS1299 #2  -> CH9-16
MCU / FPGA MOSI --------+----------- ...
MCU / FPGA MISO <-------+----------- ADS1299 #8  -> CH57-64

GPIO CS1  --------------------------> ADS1299 #1 CS
GPIO CS2  --------------------------> ADS1299 #2 CS
...
GPIO CS8  --------------------------> ADS1299 #8 CS

SYNC START --------------------------> all converters
COMMON CLK --------------------------> all converters
```

Why this architecture is preferred here:

- every ADS1299 remains independently configurable;
- fault isolation is straightforward;
- debugging is simpler;
- the same portable driver instance can be reused for every converter;
- modern MCUs and FPGAs usually have sufficient GPIO;
- the architecture scales naturally from one ADS1299 to eight devices / 64 channels.

Daisy-chain mode remains documented as an ADS1299 capability for special low-pin-count designs, but it is **not the mainstream reference architecture of this project**.

See [`docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](docs/MULTI_ADS1299_SPI_ARCHITECTURE.md).

---

## EEG analysis: Python + MATLAB + R

The repository is being developed so that hardware users can continue directly into signal analysis.

### Python

Target ecosystem:

- NumPy
- SciPy
- pandas
- matplotlib
- MNE-Python
- scikit-learn
- pyRiemann
- MOABB interoperability

### MATLAB

Target ecosystem:

- transparent starter functions
- Signal Processing Toolbox workflows where appropriate
- EEGLAB interoperability
- FieldTrip interoperability

### R

Target scope:

- reproducible statistical analysis
- visualization
- feature tables
- group-level analysis
- research reporting workflows

### Core EEG methods

- rereferencing
- notch filtering
- band-pass filtering
- PSD / Welch spectrum
- band power
- alpha-peak analysis
- Hjorth parameters
- epoching
- ERP
- artifact / QC workflows
- connectivity baselines
- feature extraction

The objective is to offer both **beginner-readable implementations** and paths toward established research ecosystems.

---

## BCI: from acquisition to reproducible baselines

BCI is a first-class part of ADS1299-Core-SDK rather than an afterthought.

### SSVEP

- FFT / PSD baseline
- harmonic SNR
- CCA
- FBCCA
- multi-frequency recognition workflows

### Motor imagery

- ERD / ERS
- CSP
- FBCSP
- LDA
- SVM
- Riemannian geometry pipelines

### P300 / ERP BCI

- epoching and baseline correction
- ERP averaging
- amplitude / latency extraction
- xDAWN roadmap
- LDA / SWLDA style baselines

### Online BCI

- ring buffers
- sliding windows
- streaming preprocessing
- online feature extraction
- classification pipeline foundations
- packet-loss detection
- latency measurement

### Advanced roadmap

- reproducible dataset examples
- MOABB-compatible benchmarking paths
- Riemannian methods
- EEGNet and selected deep-learning baselines
- hardware-to-BCI demonstrations after real ADS1299-Core validation

Each algorithm should state its evidence level, such as `Educational`, `Dataset-tested`, `Hardware-tested`, or `Online-tested`.

---

## Quality before quantity

A major repository is useful only if users can trust what the labels mean.

### Validation labels

- `Planned` — roadmap or placeholder only
- `Compiles` — the relevant toolchain build has been verified
- `Bench-tested` — tested with real ADS1299 hardware
- `24h-tested` — long-duration acquisition has been verified

### Minimum hardware acceptance path

1. Verify power rails, reference, clock and reset electrically.
2. Read the ADS1299 ID register.
3. Verify register write / readback.
4. Acquire the internal test signal on all eight channels.
5. Verify 24-bit two's-complement conversion.
6. Verify 250 SPS first, then higher configured rates.
7. Stream the common host packet and verify sequence / CRC.
8. Record packet-loss statistics.
9. Perform longer acquisition tests.
10. Only then upgrade the project's validation status.

> **No fake support. No placeholder counted as a finished port. No “works on my machine” as the final standard.**

---

## For researchers, developers and educators

This repository is intended to be useful across several communities:

- biomedical engineering
- neuroscience
- EEG / electrophysiology
- BCI / neurotechnology
- embedded systems
- FPGA development
- medical-device R&D
- digital health research
- university teaching
- student projects
- signal-processing education

If you are learning ADS1299 for the first time, start with the bring-up path. If you are an experienced embedded developer, use the common driver and a complete reference project. If you are a neuroscience or BCI researcher, start from the host, algorithm and BCI layers.

---

## Contributions

We welcome high-quality contributions from firmware engineers, FPGA developers, researchers, educators, students and companies.

Before opening a pull request, please read [`CONTRIBUTING.md`](CONTRIBUTING.md).

The core rule is:

> **Correctness, reproducibility, useful documentation and real validation matter more than raw file count.**

A good contribution should make it easier for the next person to succeed.

---

## Project positioning

Our ambition is explicit: **make ADS1299-Core-SDK the world's most comprehensive open ADS1299 ecosystem.**

We want this repository to become a natural reference point when someone searches for:

`ADS1299 STM32` · `ADS1299 GD32` · `ADS1299 CH32` · `ADS1299 ESP32` · `ADS1299 FPGA` · `ADS1299 Python` · `ADS1299 MATLAB` · `ADS1299 R` · `ADS1299 EEG` · `ADS1299 BCI` · `ADS1299 64 channel`

The strongest form of promotion is usefulness: **good code, complete projects, reproducible builds, clear documentation, real validation data, and examples that save other people days or weeks of engineering work.**

---

## Collaboration & professional support

This project is maintained by a **Biomedical Engineering / Medical PhD researcher** and is open to serious technical, academic and educational collaboration.

If your work involves ADS1299, EEG, electrophysiology, BCI, embedded acquisition or neurotechnology, collaboration is welcome in areas such as:

- **medical-device R&D collaboration** — research prototypes, acquisition electronics, embedded architecture, EEG/BCI technical evaluation and engineering discussion;
- **research and paper collaboration** — experimental systems, EEG/BCI methods, multimodal sensing, reproducible analysis and technical co-development;
- **technical support and engineering consulting** — ADS1299 bring-up, MCU/FPGA integration, multi-device acquisition, firmware architecture and debugging;
- **teaching, courses and workshops** — ADS1299, EEG instrumentation, embedded physiological-signal acquisition, signal processing and introductory BCI;
- **laboratory and university collaboration** — teaching platforms, research hardware, student projects and reproducible experimental pipelines;
- **company / product collaboration** — integration evaluation, SDK adaptation, hardware-software co-design and technical exchange.

Public questions that can benefit the community are encouraged through GitHub Issues. For collaboration, teaching, research, product discussions or direct technical contact:

**Gmail:** **yuguoyu520@gmail.com**  
**Email:** **yuguoyu123456@126.com**

> Good collaborations usually start with a concrete problem, a schematic, a dataset, an experimental goal or a clearly defined engineering requirement. You are welcome to get in touch.

---

## Intended use and responsibility

This repository is intended for **research, education, prototyping and engineering development**.

It is **not a medical device, not diagnostic software, and not a validated clinical system**. Any use in regulated medical products requires the appropriate independent engineering, risk-management, verification, validation, quality-system and regulatory work.