# LSL bridge direct-use guide

This directory is additive. Existing files are retained unchanged.

## Recommended architecture

Lab Streaming Layer should normally run on a host computer or Linux SBC, not inside the ADS1299 DRDY acquisition ISR.

Recommended data path:

`ADS1299 -> MCU/FPGA -> USB/UART/Ethernet/Wi-Fi -> host receiver -> canonical 49-byte packet validation -> LSL outlet`

This keeps hard real-time acquisition independent from liblsl networking/discovery and makes the same hardware stream reusable outside LSL.

## Files added here

- `ads1299_lsl_bridge.h/.c`: decode one canonical ADS1299 packet and convert the 8 channel codes to input-referred microvolts.
- `lsl_bridge_smoke_test.c`: verifies decode/conversion metadata and CRC rejection.
- `liblsl_outlet_example.cpp`: minimal host-side C++ outlet integration example.

## Conversion

Call:

`ads1299_lsl_packet_to_sample(packet, 49, vref_volts, gain, &sample)`

The returned structure contains:

- 8 float channel values in microvolts;
- packet sequence;
- device timestamp in microseconds;
- packet flags;
- ADS1299 status bytes.

Use the actual reference voltage and PGA gain configured on the acquisition board. Do not assume every deployment uses 4.5 V / gain 24.

## LSL channel metadata

For EEG, publish at least:

- stream type: `EEG`;
- channel count: 8 for one ADS1299 outlet, or 64 if the host aggregates eight devices into one outlet;
- nominal sample rate: the actual configured ADS1299 SPS;
- channel unit: `microvolts`;
- stable source ID per hardware stream;
- channel labels matching the electrode montage used by the experiment.

The example uses generic `EEG1..EEG8` labels only because board/electrode naming is application-specific.

## Timestamp policy

The canonical packet carries `device_timestamp_us`. That timestamp is useful for sequence/timing analysis but it is not automatically in the same time domain as `lsl::local_clock()`.

Safe baseline:

1. validate and receive the packet on the host;
2. timestamp the LSL sample with `lsl::local_clock()` at receipt;
3. keep the original device timestamp and sequence in logs for diagnostics.

For higher-quality synchronization, estimate a clock mapping between device time and host/LSL time using periodic time-pair observations, reject outliers, and map device timestamps into the LSL clock domain before publishing.

Do not directly pass raw device microseconds as an LSL absolute timestamp unless a clock-domain conversion has been established.

## 64-channel use

Two valid host designs are common:

### One outlet per ADS1299

- eight outlets;
- 8 channels each;
- simplest device separation;
- requires host-side alignment when an application wants one 64-channel matrix.

### One aggregated 64-channel outlet

- receive eight packets representing the same logical sample point;
- align them by sequence/device index and timestamp;
- build one 64-float sample;
- publish one LSL outlet with 64 channels.

For experiments that depend on tight inter-channel timing, acquisition synchronization must be solved at the hardware/FPGA/MCU layer first. LSL cannot repair unsynchronized ADC sampling after the fact.

## Loss detection

The bridge must not silently interpolate missing hardware packets.

Track:

- canonical packet CRC failures;
- packet sequence gaps;
- transport framing drops;
- host receive queue overflows;
- LSL push failures/exceptions;
- clock-mapping residuals when calibrated timestamps are used.

Whether downstream software interpolates missing EEG samples is an analysis decision, not a transport-layer behavior.

## Validation order

1. Run `lsl_bridge_smoke_test.c` against the common packet implementation.
2. Feed internal ADS1299 test-signal packets through the selected host transport.
3. Confirm the bridge reports the expected sample sequence and channel polarity.
4. Confirm Vref/gain conversion against known ADC codes.
5. Use an LSL receiver/recording application and verify channel count, nominal rate, labels and units.
6. Compare device sequence/timestamp logs with recorded LSL timestamps over a long run.
7. Only then use the bridge for synchronized multimodal experiments.

## Build boundary

`ads1299_lsl_bridge.c` itself does not require liblsl. Only `liblsl_outlet_example.cpp` requires the liblsl C++ headers/library.

This separation lets the packet/conversion layer be unit-tested even on systems where liblsl is not installed.
