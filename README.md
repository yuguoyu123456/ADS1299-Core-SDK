# ADS1299-Core-SDK

Vendor-oriented, cross-platform SDK and EEG analysis starter kit for ADS1299-based 8-channel biopotential acquisition modules.

> **Project status:** early bootstrap / pre-hardware-validation. APIs, examples and platform ports are being built. A platform is not considered supported until it has passed real ADS1299 hardware tests.

## Project goals

1. Maintain **one common ADS1299 driver** and thin platform-specific SPI/GPIO/DRDY ports.
2. Cover major MCU/SoC ecosystems used worldwide: STM32, GD32, CH32, ESP32, nRF, RP2040/RP2350, NXP, Microchip, Renesas, TI, Infineon, Silicon Labs, HC32, AT32, MM32, N32, PY32, APM32, STC and AVR.
3. Provide FPGA reference designs for AMD/Xilinx, Intel/Altera and Lattice.
4. Provide host tools and EEG analysis examples in **Python, MATLAB and R**.
5. Keep examples reproducible, documented and clearly labeled by validation status.

## Repository layout

```text
ADS1299-Core-SDK/
├── drivers/                 # common ADS1299 device driver
├── ports/                   # MCU/SoC low-level adapters
├── examples/                # board/toolchain reference projects
├── fpga/                    # Verilog reference designs and testbenches
├── host/                    # serial/USB host tools
├── algorithms/              # EEG analysis examples: Python / MATLAB / R
├── docs/                    # hardware, protocol and user documentation
└── tests/                   # parser/driver/unit/integration tests
```

## EEG analysis starter kit

The `algorithms/` tree is designed for users who want to go from raw ADS1299 samples to a first scientific analysis. Initial examples cover:

- common-average and single-channel rereferencing
- DC removal / detrending
- notch and band-pass filtering
- epoching and baseline correction
- simple amplitude-based artifact rejection
- Welch power spectral density
- canonical EEG band power (delta/theta/alpha/beta/gamma)
- individual alpha peak estimation
- time-domain statistics and Hjorth parameters
- event-related potential averaging
- magnitude-squared coherence

The roadmap also includes ICA/ASR, ERD/ERS, CSP, xDAWN, CCA/FBCCA, P300/SSVEP pipelines, PLV/wPLI, Riemannian features, microstates and machine-learning examples.

## Validation levels

- `Planned` — roadmap only
- `Compiles` — toolchain build verified
- `Bench-tested` — tested with real ADS1299 hardware
- `24h-tested` — long-run acquisition verified

Never interpret `Planned` or `Compiles` as hardware validation.

## Intended use

This repository is for research, education, prototyping and engineering development. It is **not** a medical device, diagnostic system or substitute for validated clinical software.
