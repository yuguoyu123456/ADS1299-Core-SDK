# BIAS and EEG Reference Strategy

The ADS1299 supports flexible common-mode/BIAS and reference-routing options. The core module must choose a deliberate architecture and document it so that the carrier and firmware do not create conflicting paths.

## 1. Separate three concepts

Do not use the words "ground", "reference" and "BIAS" interchangeably.

- **System ground/return**: electrical return for the electronics.
- **EEG reference electrode/path**: the signal reference used by the measurement montage.
- **BIAS/common-mode drive**: the ADS1299-supported common-mode feedback/drive path used to improve common-mode behavior when intentionally implemented.

These may be related in the circuit, but they are not automatically the same node.

## 2. Module ownership

For every board revision, state whether the following are:

- fully local to the module;
- exposed to the carrier;
- shared across multiple modules;
- unused/disabled.

Document at least:

- SRB1
- SRB2
- BIASOUT / BIASINV / BIASREF-related network as applicable
- reference-electrode connector path
- any protection/filtering on those paths

## 3. Common-reference systems

If multiple channels share one EEG reference, define exactly how it is distributed.

Review:

- whether the shared reference enters through SRB routing or external analog routing;
- whether all channels/modules see the same intended reference path;
- whether a backplane creates extra parallel paths;
- whether connector/contact impedance can unbalance channels;
- whether failure/disconnection of the reference electrode is detectable/handled.

## 4. Multi-ADS1299 BIAS architecture

Do not simply tie multiple active BIAS outputs together.

For a multi-module system, choose and validate one explicit topology, for example:

- one designated module generates the system BIAS/common-mode drive while other modules contribute sensing information only if the design supports it;
- a separate validated summing/drive architecture is used;
- BIAS is disabled on modules where it is not required.

The exact resistor network and ADS1299 register configuration must be verified against the current datasheet and the actual schematic.

## 5. Firmware ownership

Firmware must know which device owns the active BIAS/reference behavior. For 8 modules, avoid independent firmware instances enabling conflicting outputs by default.

Recommended configuration metadata includes:

- module index;
- BIAS owner flag;
- channels included in BIAS sensing;
- SRB1/SRB2 mode;
- reference topology identifier.

## 6. Safety and fault review

Human-connected BIAS/reference paths must be included in the complete patient/user protection and leakage-current analysis.

Review:

- output/input current-limiting components;
- single-fault behavior;
- connector shorts;
- electrostatic events;
- cable shield/chassis interaction;
- isolation boundary placement;
- what happens if USB/Ethernet/charger power is connected.

This software/hardware repository does not substitute for a formal electrical-safety review.

## 7. Validation

Validate the selected reference/BIAS architecture using:

1. internal test (digital/configuration baseline);
2. input short (board noise baseline);
3. known differential source;
4. representative electrode/source impedance;
5. common-mode interference test appropriate to the lab setup;
6. reference-electrode disconnect/reconnect behavior;
7. one module, then 2/4/8 modules.

Record whether BIAS is enabled and which device owns it in every test file.

## Acceptance gate

The reference/BIAS architecture is complete only when schematic ownership, connector routing, firmware ownership and multi-module behavior all describe the same topology.
