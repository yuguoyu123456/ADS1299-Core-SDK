# ADS1299 PCB Hardware & Product Releases

This directory is the public-facing release area for physical ADS1299-based PCB modules associated with the **ADS1299-Core-SDK** project.

The goal is to connect each physical board revision with reproducible design information, firmware compatibility, validation evidence, manufacturing notes, and responsible-use documentation.

## Scope

This area may include:

- ADS1299 core boards and daughterboards
- Multi-board expansion hardware for EEG/BCI research
- Board photographs and physical revision identifiers
- Schematics, PCB release packages, BOMs, and manufacturing notes intended for public release
- Electrical validation, signal-quality testing, and acceptance-test summaries
- Firmware / SDK compatibility information
- Known limitations, errata, and revision history

## Release principles

Every public hardware release should aim to provide:

1. **Traceability** — a unique hardware revision and release date.
2. **Reproducibility** — enough public information to reproduce or independently evaluate the board where licensing permits.
3. **Verification** — documented electrical and functional tests rather than marketing claims alone.
4. **Compatibility** — clear mapping to the corresponding SDK / firmware version.
5. **Transparency** — known issues and design limitations should be documented.
6. **Responsible use** — research-use and safety boundaries should be explicit.

## Recommended directory structure

- `images/` — PCB and assembled-board photographs
- `revisions/` — revision-specific documentation
- `manufacturing/` — fabrication / assembly information intended for public release
- `testing/` — validation methods, test results, and acceptance criteria
- `release_notes/` — release history and change summaries
- `templates/` — reusable hardware-release documentation templates

## Publication checklist

Before a board is publicly listed, the release should ideally document:

- Hardware name and revision
- ADS1299 configuration and supported channel count
- Power requirements and electrical interfaces
- Connector / pin definitions
- Clock, reference, reset, START, DRDY, and SPI behavior where applicable
- Firmware compatibility
- Basic electrical validation
- Input-noise / signal-path validation where applicable
- Known limitations and errata
- Design-file / documentation license status
- Research-use and safety notice

## Research-use and regulatory notice

Unless a specific release explicitly documents otherwise, hardware in this directory is intended for **research, education, prototyping, engineering evaluation, and BCI/biomedical signal-acquisition development**.

The presence of a PCB, design file, test result, or product listing in this repository does **not** by itself indicate regulatory clearance, clinical validation, medical-device certification, diagnostic suitability, or approval for treatment decisions.

See `SAFETY_AND_RESEARCH_USE.md` and `PRODUCT_RELEASE_POLICY.md` for the expected release standard.

## Relationship to the wider project

The physical hardware is intended to remain technically connected to the open-source software, documentation, EEG/BCI examples, and educational resources in the parent repository. Where feasible, each hardware release should point back to the exact firmware and documentation revision used for verification.

> **Status:** Hardware publication / product-release area under development. Commercial availability is not implied unless explicitly announced for a specific revision.
