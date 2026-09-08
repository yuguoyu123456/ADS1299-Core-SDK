# Audio/EEG timing model

A hybrid audio + EEG experiment can contain several clocks. Treat them as distinct until a documented mapping is established.

## Time domains

### EEG device time

Source: ADS1299 acquisition system canonical `timestamp_us`.

Use for:

- sample ordering within the acquisition system;
- device-side interval calculations;
- synchronization diagnostics.

Do not overwrite this with host receive time.

### Host monotonic time

Source: experiment-controller monotonic clock.

Use for:

- scheduling intervals;
- software event ordering;
- measuring host-side latency.

It is suitable for intervals because it should not be interpreted as human calendar time.

### Host wall-clock time

Use for:

- session start/end metadata;
- human-readable logs;
- cross-file organization.

Do not use it as the sole source for precise stimulus interval calculations because wall-clock adjustments may occur.

### Audio subsystem time

Some platforms expose an audio-render clock, frame counter or presentation timestamp. If available, record it as its own field and document its units/origin.

Do not assume it shares the same epoch or rate as the EEG or host monotonic clocks without calibration.

### Physical acoustic onset time

This is the time at which the acoustic stimulus is actually observed by a physical sensor at/near the intended output path.

For timing-critical ERP/auditory experiments, this is the strongest validation reference when measured through an appropriate synchronized sensor/path.

## Mapping device time to host time

A practical mapping can be represented conceptually as:

```text
host_time = offset + scale * device_time
```

where `scale` captures relative clock-rate error/drift and `offset` captures time-origin difference.

Do not assume `scale = 1` forever without measuring long-duration drift.

Keep mapping parameters/version with the recording if they are used in offline alignment.

## Event lifecycle

Recommended event log stages:

```text
SCHEDULED
DISPATCHED_TO_AUDIO
AUDIO_SUBSYSTEM_PRESENTATION (if observable)
PHYSICAL_ONSET (if measured)
```

One experiment-local `event_index`/ID should connect all stages for the same stimulus.

## Latency metrics

Useful separately reported metrics include:

```text
software dispatch latency
host-to-audio presentation latency
physical onset latency
trial-to-trial onset jitter
EEG/marker mapping residual
```

Do not collapse all of these into one generic "latency" number.

## Reconnect/restart

If the EEG device, audio process or experiment controller restarts:

- record the discontinuity;
- begin a new clock-mapping segment if needed;
- do not fit one continuous mapping across an unobserved reset.

## Multi-device / 64-channel

For 8×ADS1299, first validate that all eight EEG devices form one synchronized epoch. Audio marker alignment is a separate second problem.

Do not use audio events to conceal or repair inter-ADS1299 synchronization errors.
