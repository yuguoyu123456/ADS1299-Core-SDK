# WebSocket interoperability

ADS1299-Core uses WebSocket primarily as a browser/mobile/desktop integration layer on top of already validated acquisition data.

The implemented reference path is:

```text
integrations/open_edge_systems/linux_gateway/
integrations/mobile/web_pwa/
```

## Message family

The current gateway contract uses JSON messages with a required `type` field.

### `hello`

Connection metadata, including project/transport/channel information.

### `eeg`

One validated ADS1299 canonical packet represented as eight signed channel values plus sequence/timestamp/status metadata.

Core fields:

```text
type = "eeg"
protocol_version
device_index (recommended for explicit source identity)
flags
sequence
timestamp_us
status_hex
channels[8]
```

### `eeg_epoch`

Recommended multi-device representation for synchronized systems:

```text
type = "eeg_epoch"
sequence
device_count
channel_count
timestamps_us[]
status_hex[]
flags[]
channels[]
```

For 8×ADS1299, `channel_count` is 64.

### `status`

Operational diagnostics such as received frame count, sequence gaps and connected client count.

## Important boundary

WebSocket JSON is **not** the canonical MCU wire format. The 49-byte canonical packet remains the binary acquisition contract. A gateway validates/decodes binary transport data, then emits JSON for applications.

Do not remove canonical CRC validation merely because WebSocket/TCP already has transport integrity.

## Client compatibility

Clients should:

- dispatch on `type`;
- validate required fields and channel counts;
- tolerate additive fields;
- preserve sequence/timestamp semantics;
- treat reconnect as a possible discontinuity;
- avoid assuming WebSocket message arrival time equals acquisition time.

## Security

The repository development gateway may run without TLS/authentication on a trusted development LAN. Production/network-facing deployments require an explicit security architecture rather than exposing the development endpoint directly.
