# Direct use — MQTT

## 1. Choose a stable system ID

Example:

```text
ads1299/lab-rig-01/eeg/0
```

Keep the identifier stable for the hardware/system, not for a temporary network connection.

Avoid embedding sensitive participant or study identifiers in topics.

## 2. Raw EEG payload

For one ADS1299 source, publish the exact validated 49-byte canonical packet as the binary MQTT message payload.

Recommended device topic:

```text
ads1299/<system_id>/eeg/<device_index>
```

A subscriber then:

1. reads `device_index` from the subscribed topic/context;
2. requires exactly 49 payload bytes;
3. validates sync/version/CRC;
4. decodes sequence/timestamp/status/channels;
5. tracks gaps independently for each source.

## 3. Status payload

Example JSON shape:

```json
{
  "type": "status",
  "sample_rate_hz": 250,
  "device_count": 1,
  "sequence_gaps": 0,
  "queue_overflows": 0
}
```

Only publish fields the implementation can actually measure.

## 4. Event markers

Use a separate low-rate event topic rather than inserting text into the binary EEG payload.

Example:

```text
ads1299/<system_id>/event
```

Event payloads should carry an explicit timestamp/timebase so downstream software can align them with EEG.

## 5. Commands and acknowledgements

Keep command and data directions separate:

```text
ads1299/<system_id>/cmd/start
ads1299/<system_id>/cmd/stop
ads1299/<system_id>/ack/start
ads1299/<system_id>/ack/stop
```

A command acknowledgment should report whether the command was accepted/applied rather than assuming successful network delivery means successful hardware configuration.

## 6. QoS and retained-message policy

Choose MQTT QoS deliberately for each topic class and validate the broker/client behavior under reconnect and congestion.

Raw high-rate EEG and low-rate commands/status may require different policies.

Avoid retaining raw EEG sample messages. If retained status/configuration is used, ensure stale retained data cannot be mistaken for a live measurement.

## 7. Backpressure

MQTT publish calls belong in a network task. Use a bounded queue between acquisition and MQTT.

Define what happens when the broker/network is unavailable:

- drop newest;
- drop oldest;
- pause acquisition only when explicitly safe/desired;
- spool a bounded amount to local storage;
- mark discontinuities.

Do not allow unlimited RAM growth while disconnected.

## 8. 64-channel systems

For 8×ADS1299, calculate actual message rate:

```text
8 canonical MQTT messages per sample epoch
```

if publishing per device.

An edge gateway may instead aggregate one 64-channel epoch message, but the epoch schema must preserve source identity, sequence/timestamps and deterministic channel mapping.

## 9. Security

A production deployment should define broker authentication, authorization/ACLs and encrypted transport according to its environment.

Do not rely on topic naming alone as an access-control mechanism.
