# Analog Input Front End

This document defines the design rules for the eight ADS1299 differential input channels on the reusable core module.

## 1. Keep every channel differential and symmetric

For each channel, treat the P and N paths as one matched pair:

- CHnP path impedance should match CHnN as closely as practical;
- protection/filter components should be mirrored;
- routing length/geometry should be comparable;
- avoid adding a component to only one side unless there is a documented reason.

Mismatch converts common-mode interference into differential error and can degrade CMRR.

## 2. Input RC filter

A simple first-order RC corner is estimated by:

`fc = 1 / (2 * pi * R * C)`

Use the real component values for the selected hardware revision. There is no single universal resistor/capacitor pair for every ADS1299 module.

When selecting the network, evaluate together:

- desired EEG bandwidth;
- ADC sample rate;
- source/electrode impedance;
- input bias/leakage effects;
- ESD/protection parasitics;
- resistor thermal noise;
- capacitor tolerance/matching;
- anti-alias attenuation requirement;
- settling after electrode/contact changes.

The P and N networks should use matched nominal values and appropriate tolerance.

## 3. Protection vs noise tradeoff

Human-connected inputs usually need protection, but every added protection component can introduce leakage, capacitance, imbalance or noise.

For each protection element document:

- purpose (ESD, overvoltage, current limiting, connector transient, etc.);
- maximum expected fault condition;
- leakage in the normal EEG input range;
- capacitance;
- resistance/impedance contribution;
- symmetry between P and N;
- whether it affects input-short/noise validation.

Do not select protection only from its headline surge rating.

## 4. Input impedance and electrode compatibility

High electrode impedance increases susceptibility to interference and imbalance. The complete front end must be assessed with realistic electrode/contact impedance, not only with a function generator having a near-zero source impedance.

Test at least:

- direct/internal short baseline;
- representative source resistance on both P and N;
- intentionally mismatched source impedance;
- motion/contact changes when appropriate for the intended electrode type.

## 5. Channel numbering

Use one permanent mapping:

- connector label;
- schematic net;
- ADS1299 channel number;
- firmware channel index;
- 49-byte packet channel position;
- host display/electrode label.

A wiring/channel-order error can look like a signal-processing error later.

## 6. Avoid digital coupling

Keep these away from the input pairs and their high-impedance nodes:

- SCLK;
- USB differential pairs;
- Ethernet clocks;
- switching regulator nodes;
- antennas/RF feed lines;
- LED PWM;
- fast GPIO buses;
- oscillator outputs.

If a crossing is unavoidable, control the reference plane and avoid long parallel routing.

## 7. Guarding/shield concepts

Guarding or shielding can help in specific high-impedance designs, but it must be intentional. A poorly chosen driven guard or shield connection can create instability, leakage paths or safety problems.

Document:

- which node drives the guard/shield;
- frequency behavior;
- fault behavior;
- connector/enclosure connection;
- whether it is part of BIAS/common-mode strategy.

## 8. External reference / common reference paths

If the product uses a common EEG reference through SRB1/SRB2 or carrier routing, document exactly where the reference enters and how it is shared across channels/modules.

Do not create hidden parallel reference paths through both the module and backplane.

## 9. Validation matrix

For every PCB revision, compare all eight channels under:

1. ADS1299 internal test;
2. ADS1299 input short;
3. known differential source;
4. representative source impedance;
5. transport disabled vs enabled;
6. battery/isolated power vs the intended system power configuration when applicable.

Record raw counts before digital filters.

## 10. What this file deliberately does not hard-code

This guide does not prescribe one fixed RC pair, one universal ESD part, or one reference topology because those decisions depend on the actual product constraints and must be checked against the current ADS1299 datasheet and the full schematic.
