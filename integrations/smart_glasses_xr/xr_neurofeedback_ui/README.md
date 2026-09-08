# XR neurofeedback and BCI overlay UI

This directory defines a vendor-neutral data/UI boundary for smart-glasses and XR applications that display EEG/BCI state, neurofeedback summaries, prompts or hands-free command results.

It does not define a medical or therapeutic user interface.

## Architecture

```text
ADS1299 raw acquisition
      |
validated recording / processing
      |
EEG/BCI summary state
      |
bounded application message/queue
      |
XR overlay UI
```

Keep raw acquisition/processing off the render thread. The XR UI should consume compact validated state snapshots rather than running transport parsing or heavy EEG analysis inside frame-render callbacks.

## What belongs in an overlay state

Examples of appropriate fields include:

- EEG source sequence/timestamp reference;
- signal-valid/quality state;
- normalized research feature(s);
- classifier label/score;
- experiment prompt/state;
- event marker acknowledgement;
- source/device connectivity;
- data age/staleness.

## Stale-data protection

An XR overlay must not continue presenting an old BCI decision as if it were live after data transport/processing stops.

Every state snapshot should include enough timing metadata to detect staleness. When the maximum allowed age is exceeded, switch to an explicit stale/disconnected/neutral UI state.

## Raw EEG display

Showing raw traces can be useful for engineering, but high-channel-count waveform drawing is a separate visualization mode from participant-facing BCI feedback.

For 64 channels, avoid forcing all waveform rendering into the same critical XR frame loop if it destabilizes stimulus timing or interaction.

## Hands-free commands

A BCI classifier output is not automatically an executed command. Use a separate command-decision layer with explicit confidence/validity/refractory/confirmation rules appropriate to the research task.

Log:

- classifier decision;
- UI indication;
- command dispatch;
- command acknowledgement/result;
- timing for each stage.

## Platform boundary

Platform-specific UI/render code belongs behind an adapter. Keep the overlay state schema stable across Android-based glasses, XR runtimes and visionOS-class environments where possible.

Do not claim support for a specific device until a runnable/tested SDK path exists.
