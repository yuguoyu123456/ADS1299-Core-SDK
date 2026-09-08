# ADS1299 Core Module Validation Checklist

Use this checklist for each new module/PCB revision before integrating multiple modules.

## A. Assembly and static checks

- [ ] PCB revision and BOM revision recorded.
- [ ] Device orientation and connector orientation verified.
- [ ] No visible bridges, tombstones or missing critical parts.
- [ ] Resistance/continuity checks show no obvious power-rail short.
- [ ] Analog channel numbering matches schematic/connector labels.
- [ ] RESET/START/DRDY/SPI nets match the intended connector pins.

## B. Power-up without electrodes

- [ ] Current draw is reasonable for the assembled configuration.
- [ ] All ADS1299-related rails are measured at the device/module.
- [ ] Reference node is measured.
- [ ] Clock source is present/valid.
- [ ] RESET/PWDN default states are deterministic.
- [ ] No component shows unexpected thermal rise.

## C. Digital bring-up

- [ ] SPI Mode 1 confirmed at the pins.
- [ ] Device ID reads repeatedly and consistently.
- [ ] Register write/readback path is verified where appropriate.
- [ ] DRDY activity is observed.
- [ ] Exactly 27 bytes are captured per ADS1299-8 frame.
- [ ] 24-bit channel sign extension is correct.

## D. Internal test

- [ ] Internal-test profile is applied.
- [ ] All eight channels show the configured internal test behavior.
- [ ] Channel ordering is correct.
- [ ] No unexplained frame loss occurs.
- [ ] Host canonical 49-byte packet CRC passes.

## E. Input-short baseline

- [ ] Input-short profile is applied.
- [ ] Raw unfiltered data is recorded.
- [ ] Gain/sample rate/reference/clock metadata are stored with the capture.
- [ ] Per-channel mean, RMS/noise and peak-to-peak values are compared.
- [ ] Any channel outlier is investigated before proceeding.
- [ ] Transport disabled vs enabled comparison is performed.

## F. Known-source test

- [ ] A controlled differential source is applied using a safe bench setup.
- [ ] Amplitude scaling agrees with the configured Vref/PGA model within the expected tolerance of the test setup.
- [ ] Frequency/cadence is recovered correctly.
- [ ] Channel-to-channel gain differences are checked.
- [ ] Negative/positive polarity is preserved correctly.

## G. Reference/BIAS checks

- [ ] Reference topology matches the schematic documentation.
- [ ] SRB1/SRB2 routing/firmware mode matches the intended design.
- [ ] BIAS ownership/output behavior matches the intended design.
- [ ] No unintended parallel BIAS/reference path exists through the connector/backplane.

## H. Stress and transport interaction

Repeat acquisition while enabling the intended system load:

- [ ] USB active.
- [ ] Network/Wi-Fi/BLE active when applicable.
- [ ] MCU/FPGA processing load active.
- [ ] Long-duration continuous capture completed.
- [ ] Frame/packet loss counters recorded.
- [ ] Noise change under digital activity recorded.
- [ ] Power/reference behavior under load rechecked.

## I. Module-to-module repeatability

For a reusable core module, validate more than one assembled unit when possible:

- [ ] device ID behavior consistent;
- [ ] internal-test behavior consistent;
- [ ] input-short noise distribution compared;
- [ ] current draw compared;
- [ ] reference voltage compared;
- [ ] connector/pinout behavior identical.

## J. Release record

Before declaring the module revision ready for multi-device integration, archive:

- board revision;
- BOM revision;
- schematic/PCB release files;
- firmware commit SHA;
- measurement setup;
- sample rate/gain/reference settings;
- raw test data;
- screenshots/captures of key SPI/DRDY/clock signals;
- measured noise summary;
- known limitations/deviations.

This checklist validates engineering behavior of the core module. It does not by itself establish medical-device certification, patient isolation compliance, EMC compliance or production qualification.
