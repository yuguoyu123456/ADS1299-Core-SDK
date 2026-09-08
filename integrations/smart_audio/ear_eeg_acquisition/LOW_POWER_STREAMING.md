# Low-power streaming profile for ear-EEG research prototypes

This document defines architecture principles, not a fixed battery-life claim.

## Goal

Reduce average system power without sacrificing acquisition integrity or creating unbounded buffering.

## Acquisition side

Keep ADS1299 acquisition deterministic:

```text
DRDY
  -> capture exact frame
  -> validate/queue
  -> transport task sends later
```

Do not slow or block DRDY handling merely to wait for a radio connection window.

## Prefer explicit operating modes

A wearable can define modes such as:

```text
IDLE
CONFIGURED
STREAMING
RECORDING_LOCAL
STREAMING_PLUS_RECORDING
ERROR/RECOVERY
```

Each mode should have explicit rules for ADS1299 power/configuration, MCU clocks, radio behavior and queue ownership.

## Transport batching

Where the selected radio/OS stack supports it, batching several validated samples can reduce per-message application overhead.

However batching increases latency and queue occupancy. Measure the tradeoff instead of assuming larger batches are always better.

For BLE, use the actual application payload available after the platform's connection/attribute overhead and the repository BLE fragment header.

## Queue policy

Use a bounded queue between acquisition and radio/network transport.

Record at minimum:

- queue capacity;
- high-water mark;
- overflow count;
- selected drop policy;
- reconnect behavior.

A low-power design must not hide data loss behind unlimited buffering.

## Local recording option

If a wearable includes local nonvolatile storage, local raw recording can reduce dependence on continuous RF availability.

The implementation should still define:

- write buffering;
- filesystem/storage latency behavior;
- storage-full policy;
- metadata synchronization;
- power-failure behavior;
- later export integrity checks.

## Radio scheduling

Keep RF-heavy operations separated from sensitive analog acquisition as much as the hardware permits. Validate whether radio bursts measurably affect input-short noise or real electrode recordings.

## Sample-rate policy

Choose sample rate from research requirements first, then validate power/transport feasibility. Do not reduce sample rate solely to meet a battery target without documenting the resulting scientific limitation.

## Battery-life reporting

Any battery-life statement should record:

- battery chemistry/capacity;
- hardware revision;
- ADS1299 configuration;
- MCU/radio mode;
- sample rate/channel count;
- transport type;
- connection conditions;
- local recording state;
- measured average/peak current method;
- test duration/environment.

Do not publish a generic runtime number based only on nominal battery capacity divided by a single estimated current figure.
