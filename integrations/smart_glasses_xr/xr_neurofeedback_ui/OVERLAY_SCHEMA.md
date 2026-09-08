# XR overlay state schema

A platform adapter may serialize `OverlaySnapshot` to JSON or another local app structure. Keep field meanings stable.

Recommended logical shape:

```json
{
  "type": "bci_overlay",
  "eeg_sequence": 42,
  "eeg_timestamp_us": 123456,
  "signal_valid": true,
  "metrics": {
    "score": 0.8
  },
  "label": "target",
  "received_monotonic_ns": 1000000000
}
```

## Required semantics

### `eeg_sequence`

Reference to the ADS1299/processed EEG source sequence. Keep uint32 semantics.

### `eeg_timestamp_us`

Source/device time reference from the EEG path, not the XR render timestamp.

### `signal_valid`

Explicit processing/signal-quality validity state. UI code should not infer validity from whether a number exists.

### `metrics`

Finite numeric research metrics with documented algorithm/unit/range. Do not place undocumented clinical interpretations here.

### `label`

Optional classifier/experiment state label.

### `received_monotonic_ns`

Time at which the overlay layer received this snapshot. Used to detect stale UI state; it is not a replacement for EEG source time.

## Stale behavior

The application must define a maximum accepted state age. Once exceeded, render an explicit stale/neutral/disconnected state rather than leaving the last BCI output on screen indefinitely.

## Commands

Do not overload the overlay snapshot itself as an executed-command record.

Use separate records for:

```text
classifier decision
UI presentation
command intent
dispatch
acknowledgement/result
```

This preserves traceability in hands-free BCI experiments.

## 64-channel processing

If a metric depends on 64-channel EEG, record the preprocessing/feature configuration that created it. The overlay does not need all 64 raw values unless raw visualization is explicitly enabled.

## Privacy

Keep participant names/medical identifiers out of routine overlay/network fields. Use experiment-local identifiers according to the study's data-governance design.
