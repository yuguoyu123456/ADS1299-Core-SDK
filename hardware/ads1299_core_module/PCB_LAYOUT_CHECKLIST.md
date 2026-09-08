# PCB Layout Checklist for the ADS1299 Core Module

Use this checklist during placement/routing review before fabrication.

## 1. Partitioning

- [ ] ADS1299, reference, analog decoupling and input network form a compact analog region.
- [ ] MCU/FPGA, USB, Ethernet, RF and switching-power circuitry are outside the sensitive input region when possible.
- [ ] Analog-input connectors do not force electrode traces through digital/high-current zones.
- [ ] The board has one clear orientation and channel numbering convention.

## 2. ADS1299 placement

- [ ] Local bypass components are placed close to the associated pins.
- [ ] Reference-related components are close to their intended nodes.
- [ ] RESET/START/CLK/SPI routing does not cut through the analog input fan-in.
- [ ] Thermal/mechanical constraints do not force unnecessary long high-impedance input traces.

## 3. Differential input routing

For every CHnP/CHnN pair:

- [ ] component count is symmetric;
- [ ] nominal resistor/capacitor values are symmetric;
- [ ] trace geometry is comparable;
- [ ] long parallel exposure to SCLK/clock/RF/switch nodes is avoided;
- [ ] return/reference plane beneath the traces is controlled and continuous where appropriate;
- [ ] no unnecessary stubs are present;
- [ ] test points, if used, do not create a large capacitive antenna on a sensitive node.

## 4. Digital SPI/control routing

- [ ] SCLK has a clear return path.
- [ ] Shared SCLK/MOSI fan-out on a multi-module connector has been considered.
- [ ] CS is uniquely identifiable for the module.
- [ ] MISO contention cannot occur in the intended topology.
- [ ] DRDY is not routed next to the most sensitive analog nodes for long distances.
- [ ] START/RESET/CLK fan-out does not create uncontrolled stubs.

## 5. Power and reference routing

- [ ] rail widths/vias support measured/expected current with margin;
- [ ] bypass loops are short;
- [ ] reference node is not routed alongside high-current or clock nets;
- [ ] switching-regulator hot loops are far from inputs/reference;
- [ ] regulator feedback/sense routing follows the regulator vendor guidance;
- [ ] power entry and protection components do not inject return current through the analog input area.

## 6. Ground/return-current review

Do not approve a layout based only on AGND/DGND net names.

Review actual current paths:

- [ ] SPI return current has a continuous low-impedance path;
- [ ] USB/Ethernet/RF return current does not cross the ADS1299 input/reference region;
- [ ] switching-regulator return loops are localized;
- [ ] connector shield/chassis connections are intentional;
- [ ] analog protection discharge paths do not dump fault current through sensitive reference traces.

## 7. BIAS/reference routing

- [ ] BIAS path ownership is visible from the layout/net names.
- [ ] EEG reference path is not accidentally tied through multiple parallel routes.
- [ ] SRB-related routing matches the schematic topology.
- [ ] human-connected BIAS/reference paths include the intended protection/current-limiting components.

## 8. Connector and mechanical review

- [ ] pin-1/orientation marks are visible after assembly;
- [ ] connector polarity/keying prevents realistic reverse-insertion mistakes where necessary;
- [ ] analog and digital connectors are not easily confused;
- [ ] electrode connector channel numbering matches firmware order;
- [ ] board-to-board connector current/voltage ratings are adequate;
- [ ] test/debug connector placement does not compromise analog routing.

## 9. Multi-module use

If the core will plug into a 2/4/8-module backplane:

- [ ] identical modules can be inserted without pin conflicts;
- [ ] each module CS selection is unambiguous;
- [ ] clock direction/ownership cannot become output-to-output contention;
- [ ] BIAS ownership cannot become output-to-output contention;
- [ ] power distribution pins have enough contacts/return capacity;
- [ ] physical module spacing supports thermal and signal-integrity needs.

## 10. Pre-fabrication deliverables

Before release, save:

- schematic PDF;
- PCB top/bottom and internal-layer review plots;
- connector pin table;
- stack-up;
- impedance/critical-net assumptions where applicable;
- BOM revision;
- DRC/ERC report;
- known deviations from this checklist.

Passing PCB DRC does not prove signal integrity, noise performance or human electrical safety. Those require hardware validation.
