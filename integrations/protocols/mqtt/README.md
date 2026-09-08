# MQTT interoperability

MQTT is useful for ADS1299-Core telemetry, status, commands, event markers and distributed edge integration. It can also carry raw EEG when the broker/network/client design is deliberately sized and validated for the intended sample rate and channel count.

## Recommended topic model

Use a stable system identifier and explicit device/source identity.

Example topic family:

```text
ads1299/<system_id>/status
ads1299/<system_id>/event
ads1299/<system_id>/eeg/<device_index>
ads1299/<system_id>/epoch
ads1299/<system_id>/cmd/<command_name>
ads1299/<system_id>/ack/<command_name>
```

Do not put participant names, medical identifiers or sensitive study metadata into topic names.

## Payload choices

### Raw EEG per ADS1299

Recommended binary payload:

```text
exact 49-byte ADS1299 canonical packet
```

This preserves the repository sync/version/sequence/timestamp/status/channels/CRC contract.

### Multi-device epoch

For 8×ADS1299, either:

- publish eight canonical packets under device-specific EEG topics; or
- publish one explicitly defined 64-channel epoch payload through an edge gateway.

Do not infer device identity from message arrival order.

### Status / telemetry / commands

JSON is practical for lower-rate metadata such as:

- firmware version;
- sample rate;
- connected source count;
- sequence-gap counters;
- queue overflow counters;
- battery/edge-system state where applicable;
- start/stop/configuration acknowledgements.

## MQTT is not the acquisition ISR

Never publish to an MQTT client/broker directly from ADS1299 DRDY handling. Use a bounded acquisition-to-network queue and a separate MQTT/network task.

## Raw EEG warning

Broker throughput, retained-message behavior, QoS choice, network latency, reconnect buffering and client subscription count can all change practical performance. Measure the exact deployment rather than assuming MQTT is suitable for full-rate 64-channel raw EEG.
