# Audio neurofeedback integration

This directory defines a research-oriented closed-loop architecture in which validated ADS1299 EEG data is processed into an explicitly defined feedback score, then mapped to an audio-control parameter.

It does not claim therapeutic efficacy, diagnosis, treatment, or medical-device status.

## Closed-loop architecture

```text
ADS1299 acquisition
      |
canonical packet validation
      |
preprocessing / artifact handling
      |
feature extraction
      |
feedback score
      |
bounded feedback controller
      |
audio endpoint adapter
      |
event/timing log
```

Every stage should be independently observable so a change in audio output can be traced back to the EEG samples and algorithm that produced it.

## Separate raw EEG from feedback state

Never replace the raw EEG recording with only the feedback score. Preserve:

- canonical EEG sequence/timestamps;
- raw or reproducibly preprocessed samples;
- feature values;
- controller output;
- audio command/event markers;
- algorithm/configuration revision.

## Feedback score contract

A useful controller input is a normalized finite score in a documented range, for example:

```text
0.0 .. 1.0
```

The feature pipeline must document what the score means. A normalized score is not inherently a clinical measure.

## Output contract

The controller should generate abstract feedback parameters such as:

```text
feedback_level = 0.0 .. 1.0
```

A platform-specific audio layer can then map that to a safe, validated sound parameter.

Do not hard-code an unsafe sound-pressure or device-volume assumption in the EEG algorithm layer.

## Artifact behavior

Define what happens when the EEG is invalid or contaminated:

- freeze last feedback for a bounded period;
- move to a neutral state;
- suppress feedback;
- mark the interval invalid.

Do not continue updating feedback from known invalid samples without an explicit experiment rule.

## Timing

Record feature-window end time, controller update time and audio dispatch/event time. Closed-loop latency is a pipeline property, not just network latency.

## Safety boundary

Audio/hearing safety, participant comfort, experiment ethics, electrical isolation and any clinical claims require separate validation and governance outside this software controller.
