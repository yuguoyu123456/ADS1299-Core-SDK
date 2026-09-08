# Direct use — LSL

## Reference implementation

Use the existing host-side bridge under:

```text
firmware/transport/lsl/
```

Relevant files:

```text
ads1299_lsl_bridge.h
ads1299_lsl_bridge.c
liblsl_outlet_example.cpp
lsl_bridge_smoke_test.c
DIRECT_USE.md
```

## Recommended pipeline

```text
validated 49-byte canonical packet
        |
ads1299_lsl_bridge
        |
8-channel numeric sample + metadata
        |
liblsl outlet on host computer
```

Do not invoke liblsl APIs from MCU/FPGA DRDY handling.

## Counts vs microvolts

Prefer one of two explicit modes:

### Raw counts

Advantages:

- no dependency on assumed Vref/PGA settings;
- preserves original integer information;
- easiest to reproduce later.

### Input-referred microvolts

Use only when the acquisition configuration records the actual reference voltage and PGA gain used for that channel/device.

The repository bridge already includes conversion helpers; do not hard-code a global conversion if devices/channels can have different gain settings.

## Metadata

Before recording, define stable channel metadata such as:

```text
CH1..CH8
```

or actual electrode labels when the montage is known.

For 64-channel systems, use deterministic ordering and include enough metadata to map each global channel back to device/local channel and electrode label.

## Timestamp handling

The canonical packet contains `timestamp_us` from the acquisition side. Keep this value available for diagnostics/reproducibility.

When producing LSL timestamps, define how device time is related to the host clock used by the outlet. Do not simply assume host receive time equals sample time when transport jitter is non-negligible.

A practical system may maintain both:

- source/device timestamp;
- host/LSL timestamp.

## Event/marker streams

For ERP/BCI/multimodal experiments, publish event markers as a separate logical stream with their own documented time source. Do not encode textual event labels into EEG channel values.

## 64-channel systems

Only push one 64-channel sample when all eight ADS1299 packets for the intended epoch have been assembled and synchronized.

Recommended mapping:

```text
global_channel = device_index * 8 + local_channel
```

Keep per-device status/timestamps available for diagnostics even if the LSL sample itself is a flat 64-value vector.

## Drop detection

Continue sequence-gap tracking before the LSL bridge. LSL stream continuity alone does not prove that acquisition-side packets were never lost.
