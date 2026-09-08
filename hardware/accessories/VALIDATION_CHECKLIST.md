# Hardware Accessories Validation Checklist

Use this checklist for every released electrode adapter, cable, isolation/power accessory, backplane or auxiliary-sensor board.

## A. Identity and documentation

- [ ] Accessory revision is marked on the PCB/cable/assembly.
- [ ] Connector part numbers are recorded.
- [ ] Pinout is recorded.
- [ ] Channel/device mapping is recorded.
- [ ] Cable length/type is recorded.
- [ ] Shield/chassis connection is recorded.
- [ ] Intended maximum device/channel count is recorded.

## B. Continuity and connection integrity

- [ ] Every channel is continuity-tested end-to-end.
- [ ] No adjacent-channel short is present.
- [ ] Reference path is correct.
- [ ] BIAS/common-mode path is correct.
- [ ] Shield/drain wiring is correct.
- [ ] Reverse/offset insertion behavior is reviewed.
- [ ] Strain relief and connector retention are adequate.

## C. Electrode adapter mapping

- [ ] One channel at a time is stimulated/verified in a safe bench setup.
- [ ] Host global channel index matches the physical connector label.
- [ ] Reference electrode maps to the intended reference topology.
- [ ] BIAS electrode maps to the intended BIAS owner/output.
- [ ] No hidden parallel reference/BIAS path exists.

## D. Signal-quality checks

With the accessory installed:

- [ ] ADS1299 internal test still passes.
- [ ] Input-short baseline is compared with/without the accessory where meaningful.
- [ ] Known differential source is recovered correctly.
- [ ] 50/60-Hz pickup is assessed.
- [ ] Cable movement/microphonic artifact is assessed.
- [ ] Channel-to-channel crosstalk is assessed.
- [ ] USB/network/RF activity impact is assessed.

## E. Power/backplane checks

For each intended population (1/2/4/8 modules):

- [ ] rail voltage is measured at near and far modules;
- [ ] connector/cable voltage drop is measured;
- [ ] total current is recorded;
- [ ] thermal rise is checked;
- [ ] startup/inrush behavior is observed;
- [ ] full digital/transport load does not create unexplained acquisition loss;
- [ ] full digital/transport load does not create unexplained analog-noise increase.

## F. Digital interconnect checks

- [ ] SCLK integrity is checked at the farthest intended endpoint.
- [ ] START/RESET/clock fan-out is checked.
- [ ] CS lines address only the intended modules.
- [ ] MISO contention is absent.
- [ ] DRDY routing/aggregation works as documented.
- [ ] Long-duration packet/frame integrity passes.

## G. Isolation/safety boundary review

Before any human-connected use:

- [ ] complete power/data block diagram is reviewed;
- [ ] USB/charger/debugger/oscilloscope paths are considered;
- [ ] isolation barrier, if required, is explicitly documented;
- [ ] creepage/clearance/component-rating review is completed by the appropriate hardware process;
- [ ] leakage/fault-current verification required by the intended product/use has been performed through the appropriate safety process;
- [ ] no accessory unintentionally bypasses the intended isolation boundary.

This checklist does not itself certify electrical safety.

## H. Auxiliary sensor/IMU checks

- [ ] sensor orientation is marked/documented;
- [ ] sensor data-ready/event line works;
- [ ] timestamps use the documented time basis;
- [ ] dropped auxiliary samples are counted;
- [ ] EEG acquisition remains stable while the sensor is active;
- [ ] trigger/IMU timing offset and jitter are measured when synchronization matters.

## I. Long-duration system test

With the exact intended accessory chain installed:

- [ ] target ADS1299 device count is populated;
- [ ] target sample rate is used;
- [ ] target transport is active;
- [ ] raw data is recorded;
- [ ] CRC/sequence statistics are recorded;
- [ ] queue high-water/overflow statistics are recorded;
- [ ] connector/cable is exercised under representative movement where relevant;
- [ ] no unexplained reset/disconnect occurs.

## J. Release archive

Retain:

- accessory revision;
- schematic/pinout/cable drawing;
- BOM where applicable;
- firmware commit;
- test configuration;
- raw data;
- measured noise summary;
- signal-integrity captures where relevant;
- synchronization measurements where relevant;
- known limitations.

An accessory is complete only when the complete chain is documented and validated; a continuity check alone is not enough.
