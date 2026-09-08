# Direct use — BLE GATT

## 1. Keep protocol layers separate

Use this pipeline:

```text
ADS1299 canonical packet (49 bytes)
        |
BLE fragment encoder
        |
GATT notification/write payloads
        |
BLE stack
```

On the receiver:

```text
GATT payloads
    |
BLE fragment reassembler
    |
exact 49-byte canonical packet
    |
sync/version/CRC validation
    |
application sample
```

Do not pass each BLE notification directly to the canonical decoder unless it already contains one complete repository BLE fragment sequence that reconstructs exactly 49 bytes.

## 2. Firmware reference implementation

Use:

```text
firmware/transport/ble/ads1299_ble_frag.h
firmware/transport/ble/ads1299_ble_frag.c
```

Key APIs include:

```text
ads1299_ble_fragment_count
ads1299_ble_make_fragment
ads1299_ble_parse_fragment
ads1299_ble_reassembler_init
ads1299_ble_reassembler_push
```

## 3. Application payload sizing

Pass the **actual maximum application bytes available to one GATT data operation** to the fragment helper. Do not pass a raw controller/link headline value unless it equals the bytes your application may actually place in the characteristic payload.

The repository fragment header consumes 8 bytes.

Examples:

- application payload max = 20 bytes → 12 canonical bytes/fragment → 5 fragments for a 49-byte packet;
- application payload max >= 57 bytes → the repository fragment header plus all 49 canonical bytes can fit in one fragment.

Always obtain/derive the usable payload from the active platform/connection configuration.

## 4. Sender behavior

The sender should:

1. take one already validated/encoded 49-byte canonical packet;
2. calculate fragment count;
3. generate fragment 0..N-1 in order;
4. submit fragments from a transport task/queue;
5. stop advancing when the BLE stack reports backpressure;
6. retry according to the platform adapter policy;
7. never block ADS1299 DRDY handling while waiting for BLE capacity.

## 5. Receiver behavior

The reference reassembler is intentionally strict and in-order.

When a fragment is missing, reordered, belongs to a different packet sequence or has inconsistent fragment metadata, drop/reset the incomplete packet instead of silently concatenating unrelated bytes.

After the final fragment:

- total reconstructed length must equal 49 bytes;
- canonical packet sync/version/CRC must pass;
- only then expose the EEG sample to the application.

## 6. Device identity

The canonical packet does not contain a universal BLE device identifier. In multi-device systems, keep a stable source/device index in the BLE connection/application context.

Do not use notification arrival order to assign EEG channel identity.

## 7. 64-channel systems

For 8×ADS1299 raw streaming, calculate total fragment operations per second using the actual negotiated application payload and sample rate.

If sustained raw transport cannot be validated with adequate margin, choose a higher-throughput transport or change the architecture rather than removing CRC, dropping source identity, or allowing unbounded buffering.

## 8. Reconnect behavior

After disconnect/reconnect:

- reset any partial BLE fragment state;
- re-establish service/characteristic state;
- restart sequence-gap tracking with an explicit discontinuity marker if needed;
- do not join pre-disconnect partial data to post-reconnect fragments.

## 9. Control vs data

Keep acquisition control/configuration logically separate from high-rate EEG data. A control command path should not be able to stall the data acquisition path indefinitely.

This repository does not define universal BLE control UUIDs in this folder; platform/product implementations should document their chosen service and characteristic identifiers explicitly.
