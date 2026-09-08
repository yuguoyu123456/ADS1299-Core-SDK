# OSC interoperability

Open Sound Control (OSC) is useful for interactive media, rapid BCI prototypes, installations and experimental applications that need simple network message routing.

ADS1299-Core keeps OSC as an **integration layer**, not a replacement for the canonical acquisition packet.

## Recommended raw-data mode: OSC blob

To preserve the ADS1299-Core wire contract exactly, send one complete 49-byte canonical packet as an OSC blob.

Recommended address:

```text
/ads1299/<system_id>/raw/<device_index>
```

Payload:

```text
one OSC blob containing exactly 49 canonical bytes
```

The receiver then validates sync/version/CRC and decodes sequence/timestamp/status/channels exactly as any other ADS1299-Core host tool.

## Derived/interactive mode

For creative applications that do not need raw EEG fidelity, publish lower-rate derived values under explicit feature addresses, for example:

```text
/ads1299/<system_id>/feature/alpha
/ads1299/<system_id>/feature/beta
/ads1299/<system_id>/feature/ssvep/class
/ads1299/<system_id>/feature/p300/score
/ads1299/<system_id>/event/<name>
```

Derived features must not be confused with raw EEG samples. Document units, update rate, channel/source selection and processing method.

## Why blob mode is preferred for raw EEG

Using the canonical packet as a blob preserves:

- uint32 sequence semantics;
- uint32 device timestamp;
- three ADS1299 status bytes;
- eight signed channel values;
- flags;
- CRC16 integrity checking.

It also avoids inventing a new OSC argument layout that different OSC libraries may interpret differently.

## Transport behavior

OSC commonly runs over a datagram or stream transport depending on the application stack. The chosen transport's loss/reorder/reconnect behavior must be documented and validated separately.

## 64-channel systems

For 8×ADS1299, use explicit `device_index` in the address or publish a separately documented aggregated 64-channel message. Never assign source identity from arrival order.

For interactive feature streams, calculate features from an already synchronized 64-channel epoch rather than mixing packets from different acquisition epochs.
