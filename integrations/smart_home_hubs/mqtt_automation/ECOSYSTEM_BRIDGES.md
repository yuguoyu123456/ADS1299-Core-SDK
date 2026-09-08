# Smart-home ecosystem bridge boundary

The repository core should emit one stable MQTT intent/ack model and keep ecosystem-specific configuration in thin adapters.

## Home Assistant

Use MQTT or another documented application integration path to convert validated `automation_intent` messages into non-critical research/demo actions.

Keep Home Assistant entity/service configuration outside the BCI classifier itself. The adapter should maintain its own command allowlist and publish an `automation_ack` result.

## openHAB

Use the same intent/ack boundary. Map repository command names to openHAB items/rules/actions only in the adapter/configuration layer.

Do not make EEG channel values themselves become item commands without the explicit BCI command gate.

## Node-RED

A Node-RED flow can subscribe to the intent topic, validate the JSON schema/allowlist, route an allowed low-risk command and publish an acknowledgement.

Keep raw EEG processing outside simple actuator-flow nodes unless the entire analysis chain is deliberately implemented and validated there.

## Matter-facing bridges

If a future application bridge exposes an allowed command through a Matter-capable ecosystem, keep the BCI side at the same intent/ack boundary.

Do not claim direct Matter device support until the exact controller/bridge/API path is implemented and tested. Matter-facing behavior belongs in the platform adapter, not in the 49-byte ADS1299 packet or classifier.

## Adapter responsibilities

Every ecosystem adapter should define:

- input intent topic/schema;
- local command allowlist;
- mapping from repository command to platform action;
- target/hub identity;
- timeout/retry policy;
- acknowledgement/result mapping;
- reconnect behavior;
- user override;
- exact platform/version used for validation.

## Defense in depth

Even though `event_command_bridge/command_gate.py` already filters commands, the downstream automation adapter should validate the allowlist again.

This protects against accidental/malformed MQTT messages bypassing the classifier/gate path.

## Do not use for critical control

Generic research bridges should not be the sole control mechanism for locks, alarms, heating appliances, emergency systems, medical equipment or other hazardous/critical devices.

Prefer easily reversible, low-risk demonstration outputs with a normal manual control path.
