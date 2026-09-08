# Web/PWA Gateway Message Schema

The existing PWA consumes WebSocket **text JSON** messages from an integration gateway. The PWA does not receive the raw 49-byte ADS1299 canonical packet directly.

## EEG message

Required JSON fields:

```json
{
  "type": "eeg",
  "sequence": 123,
  "timestamp_us": 456789,
  "status_hex": "C00000",
  "channels": [0, 0, 0, 0, 0, 0, 0, 0]
}
```

Optional field currently supported by the validator:

```json
{
  "flags": 33
}
```

## Field rules

### `type`

Must equal `"eeg"`.

### `sequence`

Unsigned 32-bit integer. The gateway should preserve the device/canonical packet sequence when it has a one-to-one packet mapping.

Do not generate a new sequence number in the browser that hides device packet loss.

### `timestamp_us`

Unsigned 32-bit integer representing the timestamp carried by the current integration message. The gateway must document whether this is:

- copied from the canonical device packet;
- reconstructed/extended by the host;
- host-local acquisition time.

Do not mix time bases within one stream without metadata.

### `status_hex`

Exactly six hexadecimal characters representing the three ADS1299 status bytes, in the same byte order used by the canonical packet.

Example:

```text
C00000
```

### `channels`

Exactly eight signed int32 values.

The values should be raw ADS1299 counts sign-extended into int32, matching the canonical packet. Physical-unit conversion should be a separate display/export step using the actual Vref and PGA gain metadata.

### `flags`

Optional 0..255 bit field mirroring or deriving from the canonical packet flags when the gateway has that information.

## One ADS1299 per message

The basic PWA view is eight channels. For a 64-channel system, use one of these explicit approaches:

1. send eight device-tagged EEG messages and add `device_index` in a future schema version; or
2. define a separate multi-device group message with 64 values and timing/device metadata.

Do not silently concatenate eight devices into the current eight-channel field.

## Gateway validation order

The gateway should validate the binary/canonical source before producing JSON:

1. transport framing;
2. canonical sync/version;
3. canonical CRC;
4. sequence/timestamp/status/channel decode;
5. only then JSON conversion.

The PWA's JSON validation is not a replacement for the canonical packet CRC.

## Browser-side validation

`gateway_message.mjs` provides:

- `normalizeEegMessage()`
- `parseGatewayText()`
- `SequenceTracker`

Run the smoke test with Node:

```bash
node test_gateway_message.mjs
```

## Future schema evolution

If adding fields such as `device_index`, `sample_rate`, `gain`, electrode labels or markers:

- keep required vs optional fields explicit;
- version incompatible message changes;
- do not reinterpret an existing field with a new meaning;
- keep raw sequence/timestamp/channel values recoverable;
- update both gateway and client smoke tests together.
