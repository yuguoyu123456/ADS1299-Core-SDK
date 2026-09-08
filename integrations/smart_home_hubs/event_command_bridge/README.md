# BCI event-to-command bridge

This directory defines the safety boundary between ADS1299/BCI research outputs and a smart-home/ambient-computing automation layer.

## Core rule

Raw EEG values must not directly become actuator commands.

Use an explicit pipeline:

```text
ADS1299 EEG
  -> validated acquisition
  -> artifact/feature/classifier layer
  -> explicit command proposal
  -> command gate
  -> smart-home bridge
  -> acknowledgement/result log
```

## Command proposal

A proposal should contain enough information to audit why a command was considered, for example:

- command name;
- classifier/decision confidence;
- source EEG sequence/time reference;
- signal-valid state;
- experiment/session-local source ID.

## Gate requirements

A robust research gate can require:

- command allowlist;
- minimum confidence;
- repeated consistent proposals;
- stale-data rejection;
- cooldown/debounce after acceptance;
- invalid-signal rejection;
- explicit user override/disable state.

The gate should emit an **accepted command intent**, not directly manipulate a physical device.

## Acknowledgement

Keep these stages separate in logs:

```text
BCI proposal
command accepted by gate
command dispatched to automation system
automation acknowledgement/result
```

Network delivery success does not prove the target device actually changed state.

## Failsafe

On EEG disconnect, classifier error, stale data or bridge restart, default to no new command rather than replaying the last accepted command.

## Scope

This is for research, accessibility prototyping and demonstrations. It is not a safety-certified control path for critical equipment, locks, alarms, heating appliances, medical equipment or other hazardous systems.
