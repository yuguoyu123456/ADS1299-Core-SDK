# ADS1299-Core-SDK

> ## Aiming to build the world's #1 open ADS1299 ecosystem
> **From ADS1299 registers and complete MCU/FPGA projects to 8–64 channel systems, phones, computers, smart watches, earbuds, smart glasses/XR, smart-home hubs, Python/MATLAB/R EEG analysis, and BCI workflows — in one coherent repository.**

**ADS1299-Core-SDK** is an open engineering, research and learning ecosystem for the **Texas Instruments ADS1299** family of biopotential front ends.

The project is designed for students, researchers, embedded engineers, FPGA developers, neuroscience laboratories, BCI teams, medical-device R&D groups, educators, wearable-device teams and hardware companies that need a practical path from **ADS1299 hardware bring-up** to **reliable EEG acquisition, cross-device integration and BCI development**.

> **Mission:** become the world's most comprehensive and useful open ADS1299 learning-and-development ecosystem in breadth, depth, reproducibility and educational value.  
> **Quality rule:** roadmap breadth is never presented as verified support. A folder, platform name or marketing sentence is not counted as completion without working code, documentation and validation evidence.

---

## Start here

If this is your first visit, open **[`getting_started/`](getting_started/)**.

You do not need to understand the repository architecture first. Choose what you want to do:

| I want to... | Go here |
|---|---|
| Learn ADS1299 from the beginning | [`getting_started/`](getting_started/) |
| Understand the hardware / 8–64 channel scaling | [`hardware/`](hardware/) |
| Use STM32 / ESP32 / RP2040 / another MCU | [`firmware/mcu/`](firmware/mcu/) |
| Study or reuse the common ADS1299 C driver | [`firmware/core_driver/ads1299/`](firmware/core_driver/ads1299/) |
| Use FPGA | [`firmware/fpga/`](firmware/fpga/) |
| Receive, record or plot data on a PC | [`pc_tools/`](pc_tools/) |
| Connect ADS1299 to Android / Apple / Linux / wearables / XR / smart home | [`integrations/`](integrations/) |
| Learn EEG filtering, PSD, ERP or features | [`eeg_analysis/`](eeg_analysis/) |
| Build SSVEP / motor imagery / P300 BCI | [`bci/`](bci/) |
| Find validation rules and deeper engineering docs | [`docs/`](docs/) |
| Use public datasets with the examples | [`datasets/`](datasets/) |

---

## Why this repository exists

Many ADS1299 repositories solve only one layer: one driver, one MCU, one board, one serial script, or one research demo.

**ADS1299-Core-SDK is being built as a full-stack ADS1299 ecosystem.**

```text
ADS1299 silicon / analog front end
               │
               ▼
Register + command driver
               │
               ▼
MCU / FPGA acquisition
               │
               ▼
8 / 16 / 32 / 64-channel systems
               │
               ▼
Common packet + transport layer
               │
     ┌─────────┼───────────┬───────────┬────────────┐
     ▼         ▼           ▼           ▼            ▼
   PC/Mac    Android     iOS/iPad    Wearables   Edge/Linux
     │         │           │           │            │
     ├─────────┴──────┬────┴───────────┴─────┬──────┘
     ▼                ▼                      ▼
Smart glasses/XR   Smart audio/ear-EEG   Smart-home hubs
     │                │                      │
     └────────────────┴──────────┬───────────┘
                                ▼
                    Python / MATLAB / R EEG
                                │
                                ▼
                 BCI: SSVEP / MI / P300 / Online
                                │
                                ▼
              Research / education / prototyping / products
```

The target experience is simple:

> **Clone it → build it → connect ADS1299 → read the device ID → verify the internal test signal → stream data → open it on the device you use → analyze EEG → continue into BCI.**

---

## What makes this project different

The ambition is not to be another ADS1299 code snippet. The repository is designed around **complete-stack coverage**:

| Layer | Project direction |
|---|---|
| ADS1299 device control | readable register/command APIs, BIAS, lead-off, SRB, test signal, GPIO, frame parsing |
| MCU ecosystem | complete per-board projects across major global MCU families |
| FPGA ecosystem | AMD/Xilinx, Intel/Altera and Lattice reference tracks |
| Multi-device acquisition | practical 16/32/64-channel standard SPI architecture with independent CS |
| Host tools | common packet decoding, recording, plotting and diagnostics |
| Device integration | Android, Apple platforms, Linux, watches, earbuds, glasses/XR, smart-home and edge systems |
| EEG analysis | Python, MATLAB and R learning/research workflows |
| BCI | SSVEP, motor imagery, P300, online pipelines and reusable classifiers |
| Education | beginner-first navigation from first SPI transaction to BCI experiments |
| Quality | CI builds, explicit validation labels, reproducible toolchains and hardware evidence |

This breadth is the project's competitive advantage: **the user should not need a different unrelated repository at every step of the ADS1299 → EEG → BCI chain.**

---

## Repository structure

The repository is organized by **what the user wants to accomplish**, not by obscure internal terminology.

```text
ADS1299-Core-SDK/
│
├── getting_started/          # beginner path: first connection to first EEG/BCI
├── hardware/                 # ADS1299 module, analog design, multi-device and 64-channel design
│
├── firmware/                 # all embedded acquisition code
│   ├── core_driver/          # controller-independent ADS1299 driver
│   ├── common/               # shared embedded utilities / packet encoder
│   ├── mcu/                  # complete MCU reference projects
│   └── fpga/                 # FPGA RTL and complete reference projects
│
├── pc_tools/                 # host packet decoding, recording and visualization
│
├── integrations/             # phones, OSes, wearables, XR, smart audio, smart home, edge systems
│   ├── mobile/
│   ├── desktop_os/
│   ├── smart_watches/
│   ├── smart_audio/
│   ├── smart_glasses_xr/
│   ├── smart_home_hubs/
│   ├── open_edge_systems/
│   └── protocols/
│
├── eeg_analysis/             # EEG analysis in Python / MATLAB / R
├── bci/                      # SSVEP / motor imagery / P300 / online BCI
├── datasets/                 # dataset notes and preparation scripts
├── docs/                     # engineering documentation and validation
├── tests/                    # automated tests
└── .github/                  # CI, issue templates and project automation
```

### One MCU = one real project

The repository does not treat a small `port.c` file or an empty example folder as a complete platform.

A visible MCU reference project should contain the board entry point, build system, GPIO/SPI/DRDY integration, local ADS1299 port layer, wiring notes, exact toolchain information, bring-up procedure and expected output.

Current complete-project track:

- **STMicroelectronics** → `firmware/mcu/st/stm32f407_black/`
- **Espressif** → `firmware/mcu/espressif/esp32s3_devkitc/`
- **Raspberry Pi** → `firmware/mcu/raspberry_pi/rp2040_pico/`

Additional MCU families are added as complete, reproducible projects rather than placeholder directories.

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

See [`docs/MULTI_ADS1299_SPI_ARCHITECTURE.md`](docs/MULTI_ADS1299_SPI_ARCHITECTURE.md).

---

## Global MCU and FPGA roadmap

The controller ecosystem targets representative projects for major global platforms such as:

**STM32 · GD32 · CH32 · ESP32 · nRF52/nRF53 · RP2040/RP2350 · NXP · Renesas · TI MCU · Infineon/PSoC · Silicon Labs · Microchip · HC32 · AT32 · MM32 · N32 · PY32 · APM32 · STC · AVR**

FPGA is a separate track for **AMD/Xilinx, Intel/Altera and Lattice** families.

The roadmap is intentionally broad, but the repository distinguishes **roadmap coverage** from **verified support**.

### Validation labels

- **Planned** — roadmap only.
- **Compiles** — clean build verified with the documented toolchain.
- **Bench-tested** — verified on real ADS1299 hardware.
- **24h-tested** — long-duration acquisition verified.

See [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md) for current firmware status.

---

## Beyond the PC: full device and operating-system ecosystem

[`integrations/`](integrations/) is the cross-device layer that makes the project larger than a conventional embedded driver repository.

### Phones and tablets

- Android
- iOS / iPadOS
- HarmonyOS / OpenHarmony
- browser/PWA clients where practical

### Desktop and open operating systems

- Windows
- macOS
- Linux
- ChromeOS/Linux workflows
- Linux SBCs
- OpenWrt gateways
- Zephyr/open edge systems

### Smart watches and wearables

- watchOS
- Wear OS
- HarmonyOS/OpenHarmony wearable ecosystems
- Garmin-class devices
- Zepp OS-class devices

### Smart earbuds / headphones / ear-EEG

- wearable acquisition architectures
- BLE companion streaming
- auditory ERP/BCI synchronization
- audio neurofeedback endpoints

### Smart glasses / AR / XR

- Android-based smart glasses
- Android XR-class devices
- visionOS-class devices
- EEG + eye-tracking research paths
- SSVEP/P300 stimulus and event-marker integration

### Smart-home and ambient computing

- Home Assistant
- openHAB
- Node-RED
- MQTT
- Matter-facing application bridges where appropriate

### Common transport layer

- USB CDC / serial
- BLE GATT
- TCP / UDP
- WebSocket
- MQTT
- Lab Streaming Layer (LSL)
- OSC

The architectural goal is **one ADS1299 data model, many reliable destinations**. Device-specific apps should share channel ordering, timestamps, sequence counters, metadata and integrity semantics instead of inventing incompatible formats.

See **[`integrations/README.md`](integrations/README.md)** for the full roadmap and validation rules.

---

## PC tools and common data protocol

All controllers should present the same logical data format to upper software layers whenever practical.

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

BCI is a **first-class core layer** of this project, not an afterthought.

[`bci/`](bci/) is organized around the paradigms a learner or researcher actually wants to build.

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

The long-term objective is unusual for an ADS1299 repository: a reproducible path from **electrodes and AFE registers** all the way to **BCI algorithms and real device interfaces** in the same ecosystem.

---

## Quality policy

This repository values **correctness, reproducibility and clear documentation more than folder count**.

A platform is not called complete because a few source files exist.

A mature acquisition project should ultimately prove:

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

A mature device/OS integration should also document the exact target, transport, reconnect behavior, tested throughput, latency assumptions, packet-loss handling and real-device evidence.

If you find a mistake, unclear explanation, unsupported assumption or platform issue, please open an Issue or Pull Request. Engineering corrections are welcome.

---

## World-scale project ambition

Our explicit engineering target is to make **ADS1299-Core-SDK the world's #1 open ADS1299 ecosystem** in overall completeness and practical usefulness.

The target is not defined by repository size alone. It is defined by whether one public project can connect the complete chain:

**ADS1299 silicon → hardware → MCU → FPGA → 8/16/32/64CH → PC → Android/Apple/Linux → watches/earbuds/glasses/XR → smart-home/edge systems → Python/MATLAB/R → EEG → BCI → education → research → product prototyping.**

That combination is the project's identity.

Until public breadth, validation coverage and community adoption objectively support an unconditional ranking claim, **“world's #1” is presented as the project's explicit mission and engineering target rather than an unverified fact**. The way to earn that position is straightforward: deeper code, more real complete projects, better documentation, more hardware evidence and a community that can reproduce the work.

---

## Contributing

Firmware engineers, mobile developers, FPGA developers, biomedical engineers, neuroscientists, signal-processing researchers, BCI developers, wearable/XR developers, educators, students and hardware teams are welcome.

Please read [`CONTRIBUTING.md`](CONTRIBUTING.md) before submitting a new platform or major algorithm contribution.

---

## Collaboration & professional support

The project is maintained from a **biomedical engineering / medical research** perspective with interests spanning physiological-signal hardware, EEG/BCI, embedded systems and translational research.

Academic and engineering collaboration is welcome, including areas such as:

- ADS1299 hardware and firmware development;
- EEG / BCI system development;
- Android / Apple / Linux / wearable / XR integration;
- smart-earbud, smart-glasses and multimodal wearable research;
- multi-channel physiological acquisition systems;
- medical-device R&D and prototype development;
- research collaboration and technical discussion;
- EEG/BCI experiment and analysis consultation;
- academic paper and research-method collaboration;
- university teaching, technical courses, workshops and laboratory training;
- company / laboratory integration and custom engineering discussions.

### Doctoral Supervisor

- **Professor:** Prof. Dongchuan Yu
- **Roles:** University Professor · Doctoral Supervisor · Internationally recognized interdisciplinary scholar
- **Experience:** Extensive experience in higher education and clinical practice
- **Interdisciplinary expertise:** Biomedical Engineering · Medicine · Medical Devices · Chemistry · Physiology · Pathology

### Author

- **Name:** Guoyu Yu
- **Status:** PhD Candidate in Biomedical Engineering / Medical Sciences
- **Affiliation:** School of Medical Frontier Interdisciplinary Studies, Zhengzhou University (formerly Academy of Medical Sciences of Zhengzhou University)

For public technical questions that may help other developers, GitHub Issues are preferred. For direct collaboration or professional inquiries, email is welcome:

- **Gmail:** yuguoyu520@gmail.com
- **Email:** yuguoyu123456@126.com

---

## Intended use

This repository is intended for **research, education, prototyping and engineering development**. It is not, by itself, a certified medical device, diagnostic system or clinically validated software stack.