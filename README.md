# ADS1299-Core-SDK

Vendor-oriented, cross-platform SDK, host tooling, EEG analysis and BCI starter kit for ADS1299-based 8-channel biopotential acquisition modules.

> **Status: v0.1 bootstrap / pre-hardware-validation.** A folder existing does not mean that platform is hardware-verified. See the validation labels below.

## Why this repository exists

The goal is to let a user buy an ADS1299 core module, connect a preferred controller, prove the hardware with the ADS1299 internal/test paths, stream eight channels using one common packet protocol, and continue directly into EEG/BCI analysis.

## Hardware facts used by the SDK

- ADS1299: 8 simultaneous 24-bit biopotential channels
- SPI-compatible digital interface
- 250 SPS to 16 kSPS device data-rate range
- one conversion frame: 24-bit status + 8 x 24-bit channels = 216 bits / 27 bytes
- serial timing uses CPOL=0, CPHA=1

Always treat the current TI ADS1299 datasheet as the electrical/timing authority.

## Repository layout

```text
ADS1299-Core-SDK/
├── drivers/ads1299/         # one portable ADS1299 C driver
├── ports/                   # 20 MCU/SoC-family adapters
├── examples/                # board/toolchain reference projects
├── fpga/                    # reusable Verilog RTL + testbenches
├── host/                    # common host packet + serial tools
├── algorithms/              # EEG analysis in Python / MATLAB / R
├── bci/                     # SSVEP / motor imagery / P300 / online BCI
├── docs/                    # support matrix, validation, references
└── tests/                   # C/Python/RTL checks
```

## Global controller roadmap

The platform tree covers STM32, GD32, CH32, ESP32, Nordic nRF, RP2040/RP2350, NXP, Microchip 32-bit, Renesas, TI MCU, Infineon, Silicon Labs, HC32, AT32, MM32, N32, PY32, APM32, STC and AVR. FPGA is a separate track for AMD/Xilinx, Intel/Altera and Lattice.

See [`docs/SUPPORT_MATRIX.md`](docs/SUPPORT_MATRIX.md).

## EEG analysis

Host-side starter algorithms are provided/planned in:

- **Python** — NumPy/SciPy first; integrations planned for MNE-Python, scikit-learn, pyRiemann and MOABB
- **MATLAB** — transparent starter functions; EEGLAB and FieldTrip interoperability planned
- **R** — base-R starter analysis; package-based workflows planned

Methods include preprocessing, rereferencing, notch/band-pass filtering, PSD, band power, alpha peak, Hjorth parameters, ERP, connectivity and artifact/QC workflows.

## BCI

Current educational baselines include:

- **SSVEP:** canonical correlation analysis (CCA)
- **Motor imagery:** common spatial patterns (CSP)
- **P300:** baseline correction, ERP average, amplitude and peak latency
- **Online:** ring buffer for sliding-window pipelines

Roadmap: FBCCA, harmonic SNR, ERD/ERS, FBCSP, LDA/SVM, Riemannian features, xDAWN, EEGNet and reproducible online demonstrations.

## Validation labels

- `Planned` — roadmap/folder only
- `Compiles` — relevant toolchain build verified
- `Bench-tested` — tested on real ADS1299-Core hardware
- `24h-tested` — long-run acquisition verified

BCI examples additionally use `Educational`, `Dataset-tested`, `Hardware-tested`, and `Online-tested`.

## First hardware acceptance sequence

1. Verify power rails/reference/clock/reset electrically.
2. Read ADS1299 ID.
3. Read/write registers after leaving continuous-read mode.
4. Capture the internal test signal on all eight channels.
5. Verify correct 24-bit two's-complement decoding.
6. Verify 250 SPS first, then higher configured rates.
7. Stream the common host packet and verify sequence/CRC on Python.
8. Run a long capture before declaring the platform hardware-supported.

## Intended use

Research, education, prototyping and engineering development. This repository is **not** a medical device, diagnostic system or validated clinical software.
