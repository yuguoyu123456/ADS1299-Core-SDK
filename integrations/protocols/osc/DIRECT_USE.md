# Direct use — OSC

## Raw canonical packet mode

Recommended OSC address:

```text
/ads1299/<system_id>/raw/<device_index>
```

Send exactly one OSC blob containing one validated 49-byte ADS1299 canonical packet.

Receiver steps:

1. use the OSC address to obtain stable system/device context;
2. require one blob of exactly 49 bytes;
3. validate canonical sync/version/CRC;
4. decode sequence/timestamp/status/channels;
5. track sequence gaps per device.

This mode is preferred whenever raw EEG may need to be reconstructed later.

## Derived feature mode

For interactive applications, publish explicitly derived values at lower-rate feature addresses.

Examples:

```text
/ads1299/<system_id>/feature/alpha
/ads1299/<system_id>/feature/theta
/ads1299/<system_id>/feature/ssvep/class
/ads1299/<system_id>/feature/mi/score
/ads1299/<system_id>/event/stimulus
```

Every feature message should have documented:

- source channel(s);
- processing window;
- update rate;
- units/range;
- algorithm/version;
- timestamp/timebase if temporal alignment matters.

## Do not send raw EEG as undocumented floats

A list of floating-point values without sequence/timestamp/status/source metadata is insufficient as the primary raw-data archive format.

If a creative tool only needs simplified channel values, treat that OSC stream as a visualization/control derivative, not as the authoritative EEG recording.

## 64-channel systems

Two practical approaches:

### Per-device raw blobs

```text
/ads1299/<system_id>/raw/0
...
/ads1299/<system_id>/raw/7
```

Each message carries one canonical 49-byte packet.

### Aggregated feature stream

First assemble and synchronize all eight ADS1299 devices, then compute features from the resulting 64-channel epoch.

Do not compute a multi-channel feature from whichever eight messages happened to arrive most recently.

## Network behavior

If OSC runs over an unreliable datagram transport, preserve sequence-gap detection and define loss/reorder behavior.

If it runs over a reliable stream transport, still retain canonical sequence numbers to detect acquisition-side loss and reconnect discontinuities.

## Security

OSC address routing is not authentication. Use an appropriate network/security layer when messages cross untrusted networks.
