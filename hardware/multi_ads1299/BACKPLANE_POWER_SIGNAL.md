# Backplane Power and Signal Distribution

This document defines the carrier/backplane responsibilities for 2/4/8 ADS1299 core modules.

## 1. Backplane responsibilities

The backplane should provide a controlled, documented interface for:

- module power distribution;
- ground/return distribution;
- SPI/control fan-out;
- clock/START/RESET distribution;
- per-module CS and DRDY routing;
- host MCU/FPGA connection;
- transport interfaces such as USB/Ethernet/Wi-Fi where applicable;
- system-level isolation/protection boundary where required;
- external electrode/reference/BIAS connectors when not located on the module.

Do not hide module-specific fixes in hand-wired jumpers. If a signal is required for production use, include it in the documented backplane interface.

## 2. Power distribution

For each populated module count (1/2/4/8), document:

- total measured current;
- regulator/source capacity;
- connector/contact voltage drop;
- rail voltage at the nearest and farthest module;
- startup/inrush behavior;
- thermal rise;
- noise with all digital transports enabled.

Use measured current from the real module plus design margin. Do not scale a one-board regulator choice to eight boards by assumption.

## 3. Ground and return-current distribution

Review where current actually returns.

Separate the physical regions for:

- ADS1299 analog/reference currents;
- SPI/clock/control return currents;
- MCU/FPGA digital currents;
- USB/Ethernet/radio currents;
- switching regulator currents;
- shield/chassis currents where used.

The goal is controlled return paths and low coupling, not simply multiple ground-net labels.

## 4. Module power isolation/fault containment

Consider whether each slot needs:

- local filtering;
- ferrite/series impedance;
- load switch/current limit;
- local rail monitoring;
- removable fuse/PTC or other protection;
- enable/power-good control.

If one bad module shorts or oscillates, the failure should be diagnosable and, when practical, should not corrupt every other channel.

## 5. Clock/START/RESET fan-out

These nets may drive up to eight modules. Evaluate:

- driver fan-out capability;
- total load capacitance;
- connector/trace stubs;
- edge quality at farthest slots;
- need for a clock/fan-out buffer;
- termination/damping based on real measurement;
- power-up state before the host configures GPIOs.

Do not use multiple active drivers on one shared net.

## 6. CS and DRDY routing

Each slot should have an unambiguous identity.

Recommended naming:

- SLOT0_CS / SLOT0_DRDY
- ...
- SLOT7_CS / SLOT7_DRDY

If the backplane supports multiple SPI lanes, also document the lane per slot.

## 7. Analog/electrode routing

If analog electrode signals traverse the backplane:

- keep them away from SCLK/clock/USB/RF;
- maintain P/N symmetry;
- minimize connector transitions;
- prevent channel-order ambiguity;
- document reference and BIAS ownership;
- consider whether placing the electrode connector directly on each ADS1299 module produces a cleaner architecture.

The preferred boundary depends on mechanics and product use, but it must be intentional.

## 8. Debug and test points

Useful observability includes:

- each major rail;
- system clock;
- START;
- RESET;
- representative SCLK/MOSI/MISO;
- CS0..CS7 where practical;
- DRDY0..DRDY7 or an accessible subset plus test header;
- transport status/debug UART if used.

Avoid large analog test pads on high-impedance electrode nodes unless their effect is understood.

## 9. Slot insertion and keying

For removable modules:

- mechanically key orientation where possible;
- avoid pin arrangements where one-pin offset/reverse insertion applies harmful voltage to analog inputs;
- provide clear slot numbering;
- document hot-plug support explicitly; default assumption should be power-off insertion unless the hardware is designed and validated for hot plug.

## 10. Acceptance gate

The backplane is ready for 64-channel validation only when all eight populated slots receive stable power, shared control signals meet logic/timing requirements at the farthest slot, no bus contention exists, and enabling the full transport load does not create unexplained analog-noise or acquisition-loss problems.
