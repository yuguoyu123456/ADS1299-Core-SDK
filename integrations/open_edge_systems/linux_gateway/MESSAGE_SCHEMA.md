# Linux gateway WebSocket message schema

The existing gateway emits compact JSON messages. New edge services should keep field meanings stable so the PWA, mobile clients and other integrations can share one logical contract.

## `hello`

Example:

```json
{
  "type": "hello",
  "project": "ADS1299-Core-SDK",
  "transport": "websocket-json-v1",
  "channels": 8,
  "source_count": 1,
  "canonical_packet_bytes": 49
}
```

The existing `gateway.py` hello message also includes `server_time_unix_ms`. Clients should tolerate additive fields.

## `eeg`

One ADS1299 canonical packet becomes one eight-channel message:

```json
{
  "type": "eeg",
  "protocol_version": 0,
  "device_index": 0,
  "flags": 33,
  "sequence": 1234,
  "timestamp_us": 987654,
  "status_hex": "C00000",
  "channels": [1,2,3,4,5,6,7,8]
}
```

Rules:

- `device_index` is explicit host/integration metadata and must not be inferred from arrival order.
- `sequence` and `timestamp_us` are unsigned 32-bit values represented as JSON numbers.
- `channels` contains exactly eight signed integer ADC codes.
- `status_hex` contains exactly three ADS1299 status bytes rendered as six hexadecimal characters.
- additional diagnostic fields may be added without changing these meanings.

## `eeg_epoch`

For synchronized multi-device systems, the new `gateway_messages.epoch_message()` representation is:

```json
{
  "type": "eeg_epoch",
  "sequence": 42,
  "device_count": 8,
  "channel_count": 64,
  "timestamps_us": [1000,1001,1002,1003,1004,1005,1006,1007],
  "status_hex": ["C00000","C00100","C00200","C00300","C00400","C00500","C00600","C00700"],
  "flags": [1,1,1,1,1,1,1,1],
  "channels": [0,1,2],
  "channel_mapping": "global=device_index*8+local_channel"
}
```

The example `channels` array is abbreviated for readability; a real eight-device epoch contains exactly 64 integer samples.

An epoch is complete only when one packet from every configured device is present for the exact same sequence number. If firmware does not maintain synchronized per-device sequence numbers, use a different explicit synchronization/epoch key rather than pretending arrival order is simultaneous.

## `status`

The existing gateway supports a client request containing the literal text:

```text
status
```

and replies with a diagnostic object containing fields such as:

```json
{
  "type": "status",
  "frames": 12345,
  "gaps": 2,
  "clients": 3
}
```

Status/diagnostic messages are operational metadata, not EEG samples.

## Compatibility rules

Clients should:

1. dispatch primarily on `type`;
2. validate mandatory fields before accepting sample data;
3. tolerate additional fields;
4. reject impossible channel counts or malformed status values;
5. track sequence gaps independently of WebSocket connection state;
6. preserve raw ADC counts unless a documented Vref/PGA configuration is available for physical-unit conversion.

## Security note

JSON schema correctness is not authentication. Network-facing deployments need a separate security layer for transport encryption, client authorization and origin/access policy.
