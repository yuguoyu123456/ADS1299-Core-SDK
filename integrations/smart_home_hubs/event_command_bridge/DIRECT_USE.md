# Direct use — BCI event command gate

## Run the smoke test

```bash
python test_command_gate.py
```

Expected:

```text
smart-home command gate smoke test: PASS
```

## Configure an allowlist

Example research-only command intents:

```python
gate = CommandGate(
    {"scene_next", "scene_previous"},
    min_confidence=0.8,
    required_repeats=2,
    cooldown_ns=1_000_000_000,
)
```

The gate does not know how to control a smart-home system. It only decides whether a BCI proposal is eligible for a separate adapter.

## Feed proposals

Each proposal should include:

```text
command
confidence
source EEG sequence
signal-valid state
```

A decision is accepted only after the configured rules pass.

## User/system override

Disable new BCI command intents immediately with:

```python
gate.set_enabled(False)
```

Disabling clears the partial repeat-confirmation candidate.

Use an explicit user override in any research UI that can trigger environmental actions.

## Separate dispatch from acceptance

Correct pipeline:

```text
BCI proposal
  -> gate accepted
  -> automation adapter dispatch
  -> hub/device acknowledgement
  -> result log
```

Do not treat `decision.accepted == True` as proof the target lamp/display/audio scene actually changed.

## Safe scope

Keep prototypes limited to non-critical, reversible, easily overridden research actions.

Do not use this generic research gate as the only control path for safety-critical or hazardous systems such as locks, alarms, heating appliances, medical equipment or emergency functions.

## Logging

For every proposal/decision, record at minimum:

- host time;
- source EEG sequence/time reference;
- command;
- confidence;
- signal-valid state;
- gate decision/reason;
- later automation acknowledgement/result if dispatched.

This makes false positives and repeated/rejected decisions auditable.
