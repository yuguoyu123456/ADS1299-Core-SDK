# ADS1299-Core-SDK

> ## Building the world's most comprehensive open ADS1299 ecosystem
> **From register-level control to complete MCU/FPGA projects, multi-device acquisition, PC tools, Python/MATLAB/R EEG analysis, and BCI workflows — in one repository.**

**ADS1299-Core-SDK** is a cross-platform engineering, research and learning ecosystem for the **Texas Instruments ADS1299** family of biopotential front ends.

The project is designed for students, researchers, embedded engineers, FPGA developers, neuroscience laboratories, BCI teams, medical-device R&D groups, educators and hardware companies that need a practical path from **ADS1299 hardware bring-up** to **reliable EEG acquisition and BCI development**.

> **Project status:** active development / pre-hardware-validation for the current public reference hardware.  
> A folder is not counted as support simply because it exists. Validation status is tracked explicitly.

---

## Start here

If this is your first visit, open **[`getting_started/`](getting_started/)**.

You do not need to understand the repository architecture first. Just choose what you want to do:

| I want to... | Go here |
|---|---|
| Learn ADS1299 from the beginning | [`getting_started/`](getting_started/) |
| Understand the hardware / 8–64 channel scaling | [`hardware/`](hardware/) |
| Use STM32 / ESP32 / RP2040 / another MCU | [`firmware/mcu/`](firmware/mcu/) |
| Study or reuse the common ADS1299 C driver | [`firmware/core_driver/ads1299/`](firmware/core_driver/ads1299/) |
| Use FPGA | [`firmware/fpga/`](firmware/fpga/) |
| Receive, record or plot data on a PC | [`pc_tools/`](pc_tools/) |
| Learn EEG filtering, PSD, ERP or features | [`eeg_analysis/`](eeg_analysis/) |
| Build SSVEP / motor imagery / P300 BCI | [`bci/`](bci/) |
| Find validation rules and deeper engineering docs | [`docs/`](docs/) |
| Use public datasets with the examples | [`datasets/`](datasets/) |

---

## Why this repository exists

Many ADS1299 projects solve only one part of the problem: one driver, one MCU, one board, one script, or one research demo.

**ADS1299-Core-SDK is being built as a complete chip-to-BCI ecosystem.**

```text
ADS1299 hardware
      │
      ▼
Register / command driver
      │
      ▼
MCU / FPGA acquisition
      │
      ▼
8 / 16 / 32 / 64-channel systems
      │
      ▼
Common packet protocol
      │
      ▼
PC recording and visualization
      │
      ▼
Python / MATLAB / R EEG analysis
      │
      ▼
BCI: SSVEP / Motor Imagery / P300 / Online pipelines
```

The target experience is simple:

> **Clone it → build it → connect ADS1299 → read the device ID → verify the internal test signal → stream data → analyze EEG → continue into BCI.**

---

## Repository structure

The repository is intentionally organized by **what the user wants to accomplish**, not by internal software terminology.

```text
ADS1299-Core-SDK/
│
├── getting_started/          # beginner path: first connection to first EEG/BCI
├── hardware/                 # module hardware, multi-device and 64-channel design
│
├── firmware/                 # all embedded acquisition code
│   ├── core_driver/          # controller-independent ADS1299 driver
│   ├── common/               # shared embedded utilities / packet encoder
│   ├── mcu/                  # complete MCU reference projects
│   └── fpga/                 # FPGA RTL and reference projects
│
├── pc_tools/                 # host packet decoding, recording and visualization
├── eeg_analysis/             # EEG analysis in Python / MATLAB / R
├── bci/                      # SSVEP / motor imagery / P300 / online BCI
├── datasets/                 # dataset notes and preparation scripts
├── docs/                     # engineering documentation and validation
├── tests/                    # automated tests
└── .github/                  # CI, issue templates and project automation
```

### One MCU = one real project

The repository no longer treats a small `port.c` file or an empty example folder as a complete platform.

A visible MCU reference project should contain the board entry point, build system, GPIO/SPI/DRDY integration, local ADS1299 port layer, wiring notes, exact toolchain information, bring-up procedure and expected output.

Current complete-project track:

- **STMicroelectronics** → `firmware/mcu/st/stm32f407_black/`
- **Espressif** → `firmware/mcu/espressif/esp32s3_devkitc/`
- **Raspberry Pi** → `firmware/mcu/raspberry_pi/rp2040_pico/`

Additional MCU families will be added as complete, reproducible projects rather than placeholder directories.

---

## ADS1299 device layer

The portable C driver is located at:

**[`firmware/core_driver/ads1299/`](firmware/core_driver/ads1299/)**

The driver is intended to cover the important ADS1299 programming model without forcing application code to manipulate unexplained magic numbers.

Major areas include:

- device reset and command control;
- register read/write and register dump;
- sample-rate configuration;
- per-channel gain, MUX, power and SRB2;
- SRB1;
- BIAS configuration;
- lead-off configuration and status;
- ADS1299 GPIO;
- internal calibration/test signal;
- input-short noise test;
- continuous and command-based frame acquisition;
- 24-bit two's-complement decoding;
- ADC-code to voltage / microvolt conversion helpers.

The current Texas Instruments ADS1299 datasheet remains the electrical, timing and register authority.

---

## Multi-ADS1299 / 64-channel architecture

For 16/32/64-channel systems, the project-default architecture is **standard multi-device SPI with an independent chip-select for every ADS1299**, rather than making daisy-chain mode the normal design.

Typical shared signals:

- `SCLK`
- `DIN / MOSI`
- `DOUT / MISO`
- common clock where synchronized sampling is required
- synchronized `START` strategy

Each ADS1299 receives its own `CS` so every converter can be configured, inspected and isolated independently.

During development, keeping per-device `DRDY` observable is strongly encouraged when GPIO resources allow it.

See the detailed engineering notes in [`docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](docs/MULTI_ADS1299_SPI_ARCHITECTURE.md).

---

## MCU and FPGA roadmap

The long-term controller ecosystem includes representative projects for major global platforms such as:

**STM32 · GD32 · CH32 · ESP32 · nRF52/nRF53 · RP2040/RP2350 · NXP · Renesas · TI MCU · Infineon/PSoC · Silicon Labs · Microchip · HC32 · AT32 · MM32 · N32 · PY32 · APM32 · STC · AVR**

FPGA is a separate track for **AMD/Xilinx, Intel/Altera and Lattice** families.

The roadmap is intentionally broad, but the repository distinguishes **roadmap coverage** from **verified support**.

### Validation labels

- **Planned** — roadmap only.
- **Compiles** — clean build verified with the documented toolchain.
- **Bench-tested** — verified on real ADS1299 hardware.
- **24h-tested** — long-duration acquisition verified.

See [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md) for the current status.

---

## PC tools and common data protocol

All controllers should present the same logical data format to the computer whenever practical.

That keeps the upper software layers independent of the selected MCU or FPGA.

PC-side tools live in [`pc_tools/`](pc_tools/), while the firmware packet encoder is shared from [`firmware/common/data_packet/`](firmware/common/data_packet/).

The common packet includes sequence and integrity information so dropped frames and corrupted packets can be detected rather than silently ignored.

---

## EEG analysis: Python, MATLAB and R

[`eeg_analysis/`](eeg_analysis/) is intended to become a readable learning and research toolkit rather than a collection of disconnected scripts.

Coverage includes or is planned for:

- rereferencing;
- notch and band-pass filtering;
- FFT / PSD;
- delta, theta, alpha, beta and gamma band power;
- alpha peak analysis;
- time-domain features;
- Hjorth parameters;
- ERP extraction and baseline correction;
- artifact and quality-control workflows;
- connectivity analysis;
- feature extraction;
- classical machine learning;
- interoperability with established ecosystems such as MNE-Python, EEGLAB and FieldTrip where appropriate.

The goal is to let a beginner understand the method while still providing a path toward reproducible research workflows.

---

## BCI: from ADS1299 acquisition to brain-computer interface

BCI is a first-class part of this project, not an afterthought.

[`bci/`](bci/) is organized around the main paradigms a learner or researcher actually wants to build:

### SSVEP

- FFT / harmonic inspection
- CCA
- FBCCA
- online sliding-window recognition

### Motor imagery

- ERD / ERS
- CSP
- FBCSP
- LDA / SVM
- Riemannian approaches
- online classification

### P300

- ERP extraction
- amplitude and latency
- xDAWN roadmap
- classification pipelines
- online P300 workflow

### Common BCI methods

- classical classifiers;
- Riemannian geometry;
- evaluation and cross-validation;
- online ring buffers and windowing;
- future deep-learning baselines where they add real value.

The long-term objective is a reproducible path from **ADS1299 electrodes and firmware** to **research-grade BCI experiments** in the same ecosystem.

---

## Quality policy

This repository values **correctness, reproducibility and clear documentation more than folder count**.

A platform is not called complete because a few source files exist.

A mature reference project should ultimately prove:

1. clean build from documented tools;
2. correct SPI Mode 1 operation;
3. ADS1299 hardware reset;
4. device-ID read;
5. register read/write;
6. internal test signal on all eight channels;
7. correct 27-byte frame acquisition;
8. correct signed 24-bit conversion;
9. packet sequence / integrity checking;
10. documented wiring and expected output;
11. real-hardware validation before `Bench-tested` status;
12. long-run acquisition before `24h-tested` status.

If you find a mistake, unclear explanation, unsupported assumption or platform issue, please open an Issue or Pull Request. Engineering corrections are welcome.

---

## Project ambition

Our ambition is to make **ADS1299-Core-SDK the largest and most complete open ADS1299 learning-and-development ecosystem in the world** — not by collecting empty folders, but by connecting the entire stack:

**ADS1299 → hardware → firmware → FPGA → multi-device systems → PC tools → EEG → BCI → education → research.**

Until breadth, validation coverage and community adoption objectively support the strongest wording, we describe this as the project's mission and engineering target.

The repository should become valuable because people can genuinely learn from it, build from it and contribute back to it.

---

## Contributing

Firmware engineers, FPGA developers, biomedical engineers, neuroscientists, signal-processing researchers, BCI developers, educators, students and hardware teams are welcome.

Please read [`CONTRIBUTING.md`](CONTRIBUTING.md) before submitting a new platform or major algorithm contribution.

---

## Collaboration & professional support

The project is maintained from a **biomedical engineering / medical research** perspective with interests spanning physiological-signal hardware, EEG/BCI, embedded systems and translational research.

Academic and engineering collaboration is welcome, including areas such as:

- ADS1299 hardware and firmware development;
- EEG / BCI system development;
- multi-channel physiological acquisition systems;
- medical-device R&D and prototype development;
- research collaboration and technical discussion;
- EEG/BCI experiment and analysis consultation;
- academic paper and research-method collaboration;
- university teaching, technical courses, workshops and laboratory training;
- company / laboratory integration and custom engineering discussions.

For public technical questions that may help other developers, GitHub Issues are preferred. For direct collaboration or professional inquiries, email is welcome:

- **Gmail:** yuguoyu520@gmail.com
- **Email:** yuguoyu123456@126.com

---

## Intended use

This repository is intended for **research, education, prototyping and engineering development**. It is not, by itself, a certified medical device, diagnostic system or clinically validated software stack.
