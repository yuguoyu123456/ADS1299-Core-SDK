# XR visual-stimulus synchronization

This directory covers research applications in which smart glasses/AR/XR displays present visual stimuli while ADS1299 EEG is acquired separately.

Primary examples include SSVEP, P300/ERP, visual attention tasks and experiment prompts.

## Core rule

A render-loop callback or UI update request is not automatically the physical display onset seen by the participant.

Keep separate timing concepts:

```text
stimulus scheduled
render/update submitted
display frame/presentation timestamp (if exposed)
physical visual onset measurement (if measured)
EEG device timestamp
host receive time
```

## Architecture

```text
experiment controller
    |                 \
    | stimulus command \ event log
    v                   \
XR/AR rendering ---------+---- timing record
                         /
ADS1299 acquisition ----/
       |
sequence + timestamp
```

## SSVEP

For SSVEP, the effective visual modulation frequency depends on the actual display presentation sequence, not only on a software timer.

Document:

- intended stimulus frequency;
- display refresh behavior;
- frame/presentation schedule;
- dropped/repeated frame detection when available;
- measured effective stimulus timing where required.

Do not claim an exact SSVEP frequency from nominal frame-rate text alone without verifying the actual frame sequence.

## P300 / ERP

For P300/ERP paradigms, log one explicit event ID for every stimulus onset/condition. Preserve the relationship between:

- event ID;
- stimulus identity/class;
- software dispatch time;
- presentation timing information;
- EEG sequence/timestamp.

## Platform boundary

Android/XR/visionOS-class platforms expose different rendering APIs. Keep platform-specific presentation callbacks behind an adapter and keep the experiment event schema stable.

Do not claim a device-specific timing capability until the exact hardware/OS/SDK path has been tested.

## Physical validation

When timing accuracy is scientifically important, validate visual onset with an independent measurement method appropriate to the display/setup, such as a synchronized photodetector/reference sensor.

Software timing alone does not prove photon onset at the eye.
