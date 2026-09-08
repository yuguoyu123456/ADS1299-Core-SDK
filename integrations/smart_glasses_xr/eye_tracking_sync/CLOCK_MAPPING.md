# EEG / eye-tracker clock mapping

EEG and eye-tracking systems should be treated as independent clocks until a mapping is measured.

## Linear mapping model

A first-order mapping can be represented as:

```text
common_time_ns = offset_ns + scale * source_time_ns
```

where:

- `offset_ns` accounts for different clock origins;
- `scale` accounts for relative clock-rate error/drift.

The helper in `multimodal_sync.py` implements this representation but does not estimate the parameters automatically.

## Estimating a mapping

Possible calibration evidence can include:

- shared hardware trigger observed by both systems;
- repeated paired synchronization events;
- platform-provided clock-correlation APIs;
- synchronized external sensor events.

Document the exact method used by the experiment.

## Do not use one offset forever without checking drift

A clock pair that is aligned at session start may drift over a long recording. For long sessions, evaluate mapping residuals over time and use segmented/re-estimated mappings when needed.

## Arrival time is not source time

Network/USB receive timestamps include transport and scheduling jitter. They may be useful diagnostic observations but should not automatically replace device/source timestamps.

## Alignment tolerance

After clocks are mapped, choose a maximum allowed EEG-to-eye time difference based on the scientific task and eye-tracker sample rate.

The repository helper requires an explicit `max_delta_ns`. If no valid gaze sample lies within the tolerance, the aligned gaze value is missing.

Do not silently reuse the last gaze sample unless the analysis explicitly defines a hold-last-sample policy.

## Invalid eye samples

Eye trackers may mark samples invalid because of blink, tracking loss or geometry. Preserve that validity information.

An invalid sample should not become valid merely because its timestamp is close to an EEG sample.

## Event-centric validation

For visual ERP/SSVEP experiments, use shared stimulus/event markers to validate the continuous clock mapping:

```text
visual event
  -> EEG timing reference
  -> eye-tracker timing reference
  -> mapped residual
```

This provides a direct check that the multimodal alignment remains consistent during the actual task.

## 64-channel EEG

Map time only after the intended 64-channel EEG epoch has been assembled. If eight ADS1299 devices disagree in timing, preserve that diagnostic information instead of assigning one eye sample to a falsely synchronized epoch.
