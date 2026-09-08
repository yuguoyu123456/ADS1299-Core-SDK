# ADS1299 Hardware Product Release Policy

This policy defines the minimum documentation expected before a physical ADS1299 board is presented as a public release associated with ADS1299-Core-SDK.

## 1. Release identity

Each release should state:

- Product / board name
- Hardware revision
- Release date
- PCB revision marking
- Compatible firmware / SDK revision
- Status: prototype, engineering sample, beta, or released hardware

## 2. Required technical documentation

A public release should provide, where applicable and legally permitted:

- Schematic or a clearly documented electrical architecture
- Pinout and connector definitions
- Power-supply requirements and limits
- ADS1299 clock / reference configuration
- SPI and control-signal interface information
- Channel configuration and supported sampling modes
- BOM or component-class information
- Assembly notes
- Mechanical dimensions
- Firmware compatibility

## 3. Verification evidence

A release should distinguish between **designed specifications** and **measured results**.

Recommended evidence includes:

- Power-rail measurements
- Startup / reset behavior
- SPI communication verification
- DRDY / sampling verification
- Internal-test-signal acquisition
- Input-short / noise testing
- Channel-to-channel consistency checks
- Multi-board synchronization checks when relevant
- Known failure modes and errata

Measured values should state the test configuration and instruments where practical.

## 4. Change control

Changes that can affect electrical behavior, signal quality, safety, compatibility, or manufacturability should result in a documented revision.

Release notes should summarize:

- What changed
- Why it changed
- Whether firmware compatibility changed
- Whether previous test results remain applicable
- Any migration steps for users

## 5. Responsible claims

Do not describe a board as clinically validated, medically approved, diagnostic, therapeutic, or certified unless that status is supported by the relevant formal evidence.

Engineering validation and research performance should be described using reproducible test results rather than unsupported superiority claims.

## 6. Commercial listings

If hardware is later sold or distributed, the public listing should identify the exact hardware revision and should not create ambiguity between:

- open-source reference designs,
- engineering prototypes,
- commercially assembled boards, and
- regulated medical devices.

Commercial availability does not change the open-source or documentation license automatically; each applicable license should be stated explicitly.

## 7. Traceability

Where feasible, every released board should be traceable to:

`hardware revision -> manufacturing package -> test procedure -> firmware/SDK version -> release notes`

This traceability is intended to improve reproducibility, research confidence, and long-term maintainability.
