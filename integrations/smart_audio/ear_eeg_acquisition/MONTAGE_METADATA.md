# Ear-EEG montage metadata

Ear-EEG recordings are difficult to reproduce if channel labels are reduced to `CH1..CH8`. Store explicit montage metadata alongside each recording/session.

## Minimum per-channel metadata

Recommended fields:

```text
channel_index
channel_label
device_index
ads1299_local_channel
input_positive_label
input_negative_label
reference_role
bias_role
side_or_location
sample_rate_hz
pga_gain
units
```

Optional but useful fields include:

```text
electrode_type
electrode_material
connector_position
fit/session note
measured contact/impedance metric if the system supports it
```

## Device/channel mapping

For multi-device systems, keep stable hardware identity.

Recommended global mapping:

```text
global_channel = device_index * 8 + ads1299_local_channel
```

Do not renumber channels after packet loss or device reconnect.

## Reference metadata

Record the actual reference scheme used by the experiment/system. Examples of descriptive metadata concepts include:

- dedicated reference electrode;
- common reference shared by selected channels;
- differential local pair;
- re-referenced offline during analysis.

Do not label a channel as referenced to an electrode unless the actual hardware/software routing supports that statement.

## BIAS metadata

Store whether BIAS/common-mode drive is:

- enabled/disabled;
- driven by one designated ADS1299/system node;
- connected to a specific electrode/location;
- intentionally unused in a given prototype.

For multiple ADS1299 devices, never infer that all BIAS outputs were safely paralleled merely because recordings were obtained.

## Session metadata

Ear fit can materially affect artifact and contact. Useful session-level fields include:

```text
left/right device placement
insert/reseat events
participant movement task
audio playback state
cable/wireless configuration
firmware revision
hardware revision
```

## Storage principle

Keep metadata machine-readable where practical (JSON/CSV/sidecar structure) and also preserve a human-readable experiment note. The raw 49-byte packet does not contain the full montage description, so recording only raw packet files is not sufficient for reproducible ear-EEG research.
