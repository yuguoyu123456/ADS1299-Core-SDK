# ADS1299 Core Module

This folder defines the project-level hardware contract for one reusable ADS1299 core module.

The goal is a module that can be used alone for 8-channel bring-up and replicated on a carrier/backplane for 16/32/64-channel systems.

## Design priorities

1. Keep the ADS1299 analog front end self-contained and repeatable.
2. Expose only the digital/control and power signals that the carrier actually needs.
3. Keep analog inputs physically separated from clocks, USB, RF and high-current switching paths.
4. Make one module fully testable before multi-module integration.
5. Do not make the module dependent on fixed MCU GPIO numbers.

## Recommended interface groups

### Power

Expose only rails intentionally supplied by the carrier. The exact rail architecture is board-specific, but the connector/pinout documentation must clearly distinguish analog, digital and ground domains.

### SPI/control

The normal project interface should account for:

- SCLK
- MOSI / DIN
- MISO / DOUT
- CS
- DRDY
- START
- RESET
- CLK input/output strategy when required
- PWDN if the carrier must control it

Optional ADS1299 GPIO/lead-off/reference-related pins should be exposed only when the system architecture needs them. Avoid making every silicon pin a mandatory backplane signal.

### Analog inputs

Provide eight clearly indexed differential channel pairs:

- CH1P / CH1N
- ...
- CH8P / CH8N

Keep channel numbering identical across schematic, PCB silkscreen, connector, firmware and host software.

## Core-module boundary

A reusable core module should normally contain the components required for the ADS1299 itself to operate correctly, including the device-local decoupling/reference/clock/input network chosen by the hardware design.

The carrier/backplane should normally handle:

- MCU/FPGA/Linux host;
- USB/Ethernet/Wi-Fi/BLE interfaces;
- system power entry and isolation architecture;
- multi-module fan-out;
- external electrode connectors;
- system-level protection/enclosure.

## Bring-up sequence

Use `getting_started/` and validate one module in this order:

1. power/clock/reset;
2. SPI ID;
3. internal test;
4. input short/noise;
5. 250 SPS EEG profile;
6. PC packet validation;
7. only then multi-module integration.

## Files in this folder

- `PINOUT_AND_CONNECTOR.md` — module/carrier signal contract.
- `POWER_CLOCK_REFERENCE.md` — supply, clock and reference design checks.
- `ANALOG_INPUT_FRONTEND.md` — differential input/filter/protection design rules.
- `BIAS_REFERENCE_STRATEGY.md` — BIAS and EEG reference architecture.
- `PCB_LAYOUT_CHECKLIST.md` — placement/routing checklist.
- `VALIDATION_CHECKLIST.md` — module-level engineering acceptance test.

This repository does not claim that a board is electrically safe or medically certified merely because it follows these files. Verify the complete hardware against the current ADS1299 datasheet and the applicable safety requirements of the end product.
