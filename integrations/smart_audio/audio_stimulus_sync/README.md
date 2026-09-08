# Audio stimulus synchronization

This directory covers hybrid research setups where ADS1299 EEG acquisition and audio stimulus presentation run through separate hardware/software paths and must be aligned afterward or during the experiment.

## Core rule

Do not treat an audio API call, UI button press, or network command time as proof that acoustic energy reached the ear at that exact instant.

Keep distinct timing concepts:

```text
stimulus scheduled time
stimulus software-start/callback time
hardware/audio-output time estimate (if available)
physical acoustic onset measurement (when measured)
EEG device timestamp
host receive time
```

## Recommended architecture

```text
experiment controller
    |                         \
    | event command            \ marker log
    v                           \
audio output endpoint           -> timing record
                                /
ADS1299 acquisition ---------- /
        |
canonical sequence + timestamp
```

The experiment controller should generate an explicit event ID for each stimulus so the audio log and EEG/marker stream can be reconciled without relying only on filenames or wall-clock timestamps.

## Event identity

Each stimulus event should include at minimum:

- monotonically increasing event sequence or UUID-like experiment-local ID;
- event type/name;
- stimulus identifier;
- host monotonic timestamp at scheduling/dispatch;
- optional audio subsystem timing information;
- optional measured physical onset timestamp;
- experiment/session metadata reference.

## Timebase principle

Prefer monotonic clocks for interval/timing measurements. Wall-clock time is useful for human-readable session metadata but can jump because of synchronization or system changes.

Keep the ADS1299 `timestamp_us` unchanged in acquisition records. If mapping device time to host time, store the mapping strategy/parameters rather than overwriting source timestamps.

## Physical validation

For experiments where millisecond-level stimulus timing matters, validate the end-to-end audio path using an independent physical measurement appropriate to the setup, such as a microphone/sensor/reference channel synchronized to the acquisition system.

Software timestamps alone do not establish acoustic onset latency.
