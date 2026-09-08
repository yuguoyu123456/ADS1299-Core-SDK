# EEG + eye-tracking synchronization for smart glasses/XR

This directory defines a vendor-neutral synchronization layer for combining ADS1299 EEG with eye-tracking samples from smart glasses, XR headsets or companion eye trackers.

It does not assume that any specific device exposes gaze data or a particular timestamp API.

## Core rule

Do not merge EEG and gaze by list index or arrival order.

The two systems can have different:

- sample rates;
- clocks/time origins;
- transport paths;
- buffering;
- packet loss;
- latency/jitter.

Before pairing samples, map timestamps into one documented common time domain.

## Architecture

```text
ADS1299 canonical packets ----> EEG source time ----+
                                                 mapping
Eye tracker samples ----------> eye-tracker time ----+
                                                    |
                                                    v
                                             common timebase
                                                    |
                                             bounded alignment
                                                    |
                                      synchronized multimodal rows/events
```

## Eye sample metadata

A useful gaze sample should preserve available fields such as:

- source timestamp;
- gaze x/y or direction/vector;
- validity flag;
- eye identifier when available;
- pupil metric when available;
- head pose or confidence when available;
- source device/SDK identity.

Do not invent unsupported fields for a device.

## Alignment policy

A simple research alignment policy can use nearest-neighbor matching within an explicit maximum time difference.

If no eye sample falls inside that tolerance, mark gaze as missing rather than reusing a distant sample silently.

Interpolation/resampling may be appropriate for some analyses, but it must be an explicit offline/processing choice with documented assumptions.

## Event-centric alignment

For ERP/P300/SSVEP tasks, it can be more reproducible to align both EEG and gaze to the same experiment event IDs/timing model rather than only pairing sample streams continuously.

## 64-channel EEG

Assemble the intended synchronized 64-channel EEG epoch first, then align the epoch timestamp to gaze. Do not let eye-tracker timing hide inter-ADS1299 synchronization problems.
