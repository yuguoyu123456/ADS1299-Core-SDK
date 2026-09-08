# MQTT smart-home automation bridge

This directory defines a platform-neutral MQTT bridge between accepted BCI command intents and smart-home/ambient-computing software.

It builds on:

```text
integrations/protocols/mqtt/
integrations/smart_home_hubs/event_command_bridge/
```

## Architecture

```text
BCI classifier
   |
command gate
   |
accepted command intent
   |
MQTT automation bridge
   |
Home Assistant / openHAB / Node-RED / other application bridge
   |
non-critical research automation
```

## Recommended topic family

```text
ads1299/<system_id>/automation/intent
ads1299/<system_id>/automation/ack
ads1299/<system_id>/automation/status
ads1299/<system_id>/automation/event
```

Keep participant names/medical identifiers out of topic names.

## Command-intent payload

A command intent should carry enough information to audit the BCI decision without exposing raw EEG unnecessarily.

Example logical fields:

```text
schema_version
intent_id
command
source_sequence
confidence
host_monotonic_ns
```

The MQTT subscriber/automation layer should validate the command against its own allowlist as defense in depth.

## Acknowledgement

The downstream adapter should publish a separate acknowledgement/result message containing:

- same `intent_id`;
- accepted/rejected/dispatched/result state;
- target adapter/hub identifier;
- host timestamp;
- optional non-sensitive detail/error.

Do not report a target action as successful merely because the MQTT publish succeeded.

## Ecosystem boundary

Home Assistant, openHAB, Node-RED and Matter-facing bridges should be treated as adapters around this stable intent/ack model. Exact platform APIs/configuration can evolve; keep repository core logic independent of one integration version.

## Raw EEG

Raw EEG may use the generic MQTT protocol integration for telemetry/streaming experiments, but it should not be consumed directly by actuator automations.
