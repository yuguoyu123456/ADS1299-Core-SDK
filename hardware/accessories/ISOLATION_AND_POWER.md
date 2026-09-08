# Isolation and Power Boundary

This document is a design-review checklist for human-connected EEG systems. It is not a certification standard and does not declare any board safe for human connection.

## 1. Draw the complete power/data path

For the real setup, draw one block diagram that includes:

- electrodes and BIAS/reference electrodes;
- ADS1299 module(s);
- MCU/FPGA;
- battery/regulators;
- USB connection;
- Ethernet connection;
- charger/programmer/debugger;
- laptop/desktop mains connection;
- auxiliary sensors;
- shields/chassis.

Safety review must consider every connection present during use, not only the ADS1299 PCB.

## 2. Define the isolation boundary

If galvanic isolation is required by the product/use case, document:

- where the isolation barrier is located;
- which power rails cross it;
- which digital signals cross it;
- isolation component ratings/creepage/clearance requirements;
- whether USB/Ethernet/debug/programming can bypass the intended barrier;
- what happens while charging a battery.

Do not place an isolator in the schematic and assume the complete system is isolated.

## 3. Power options

Common engineering approaches include:

- battery-powered isolated acquisition during recording;
- isolated DC/DC plus digital isolation;
- system-level medically appropriate power/isolation architecture.

The correct choice depends on the intended product and applicable standards. Verify the complete design with qualified safety review/testing.

## 4. Leakage and fault paths

Review all human-connected nodes for possible current paths through:

- electrode inputs;
- BIAS/common-mode electrode;
- EEG reference electrode;
- ESD/protection devices;
- USB shield/ground;
- Ethernet shield/magnetics/chassis;
- charger/programmer/debug probe;
- auxiliary-sensor cables;
- enclosure metal.

Single-fault behavior matters; normal operation alone is not enough.

## 5. Debugging caution

A prototype that appears isolated during battery operation may lose that isolation as soon as a grounded oscilloscope, USB cable, programmer or debugger is attached.

Before human connection, explicitly define whether each debug instrument may be connected and how it affects the isolation boundary.

## 6. Isolation vs signal quality

Isolation design also affects noise:

- isolated DC/DC switching can inject common-mode noise;
- digital isolators can add edge-current transients;
- shields can create unintended return paths;
- poor barrier/layout design can couple clocks into the analog front end.

Validate input-short noise with the exact final isolation/power configuration.

## 7. Verification records

For each hardware revision, retain:

- isolation block diagram;
- schematic pages covering the barrier;
- component part numbers/ratings;
- creepage/clearance review;
- power-state matrix;
- charger/debugger connection rules;
- leakage/safety test results performed by the appropriate process;
- input-short noise comparison across power modes.

## 8. Repository boundary

Firmware tests, internal ADS1299 test signals and successful EEG plots do not establish electrical safety. Do not label a system "human safe", "medical grade" or "certified" unless the complete hardware has actually passed the required engineering/regulatory process.
