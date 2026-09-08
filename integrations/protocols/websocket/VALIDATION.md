# WebSocket validation checklist

## Connection and schema

- [ ] client receives a valid `hello` message;
- [ ] `eeg` messages contain exactly 8 channels;
- [ ] `sequence` and `timestamp_us` remain valid uint32-compatible values;
- [ ] `status_hex` has the expected six hexadecimal characters;
- [ ] clients tolerate additional fields;
- [ ] malformed JSON is rejected without crashing the client;
- [ ] unknown message types are ignored or handled explicitly.

## Reconnect and discontinuity

- [ ] disconnect/reconnect does not join stale partial application state;
- [ ] sequence discontinuity is visible after reconnect when data was missed;
- [ ] client resets any connection-local state that must not survive reconnect;
- [ ] gateway client count/status remains accurate after repeated reconnects.

## Slow-client/backpressure

- [ ] a slow client does not block acquisition ingestion;
- [ ] per-client buffering is bounded or equivalent flow control exists;
- [ ] overload behavior is explicit;
- [ ] dropped/disconnected slow clients are observable;
- [ ] long-duration memory use remains bounded.

## 64-channel

- [ ] `eeg_epoch.device_count == 8` for an eight-device system;
- [ ] `channel_count == 64`;
- [ ] `channels.length == 64`;
- [ ] per-device timestamp/status arrays contain eight entries;
- [ ] global channel mapping is deterministic;
- [ ] missing source/device data does not silently shift later channels.

## Security/deployment

For any deployment outside a trusted development environment:

- [ ] TLS/WSS is configured where required;
- [ ] authentication/authorization is defined;
- [ ] origin/access policy is defined;
- [ ] exposed host/interface binding is deliberate;
- [ ] logs avoid leaking sensitive participant/study metadata;
- [ ] rate/connection limits are considered.

## Qualification record

Record:

- gateway hardware/OS;
- gateway software revision;
- client browser/app and version;
- transport source feeding the gateway;
- sample rate/channel count;
- test duration;
- sequence gaps;
- client reconnect count;
- dropped outbound messages where applicable;
- CPU/memory usage.

Passing this checklist validates the WebSocket integration only, not ADS1299 analog quality, hardware synchronization, electrical isolation or clinical/medical safety.
