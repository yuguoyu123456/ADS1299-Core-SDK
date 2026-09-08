# XR visual timing model

Visual BCI/ERP timing should be described as a chain of observable stages rather than one assumed timestamp.

## Time domains

Keep these distinct where available:

- EEG device timestamp;
- experiment-controller monotonic time;
- XR render-loop time;
- platform presentation/display timestamp;
- physical visual onset measurement;
- eye-tracker timestamp.

## SSVEP frame scheduling

If a stimulus is implemented by alternating visual states on display frames, define the exact intended frame sequence.

For an intended display refresh rate `F` and a pattern repeating every `N` frames, the nominal pattern repetition frequency is:

```text
F / N
```

But the scientific stimulus frequency should be verified against the actual presented frame sequence/physical output when timing accuracy matters.

Track, when possible:

- intended frame index;
- actual presentation timestamp;
- dropped/repeated/missed presentation events;
- state shown on that frame.

## P300/ERP events

Every target/non-target or stimulus-class transition should carry a unique event ID. The event log should preserve the distinction between:

```text
experiment decision time
render submission time
presentation time
physical onset time
```

ERP epoching should use a documented timing reference rather than whichever callback is easiest to access.

## Host-to-EEG mapping

Keep ADS1299 `timestamp_us` as source time. If a host monotonic timestamp is mapped to the EEG timebase, record mapping parameters and drift/residual metrics.

## Eye tracking

If gaze is used to interpret SSVEP/P300 performance, eye-tracker timestamps form another independent time domain. Synchronize/align them explicitly rather than using UI frame arrival order.

## Reprojection / asynchronous display pipelines

XR systems may internally decouple application rendering from final display presentation. Therefore application render timing should not automatically be reported as physical display timing unless the platform/API and measurement support that claim.

## Physical validation

A synchronized photodetector or equivalent optical measurement can be used to characterize end-to-end visual onset latency/jitter for timing-critical research.

Store the measurement setup and calibration with validation records.
