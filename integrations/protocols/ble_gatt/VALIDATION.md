# BLE GATT validation checklist

## Fragment protocol

- [ ] fragment magic is `0xB9`;
- [ ] packet sequence is decoded little-endian;
- [ ] fragment index is zero-based;
- [ ] fragment count is 1..16;
- [ ] payload length matches the actual fragment size;
- [ ] all fragments of one packet use the same packet sequence/count;
- [ ] reconstructed length is exactly 49 bytes;
- [ ] canonical sync/version/CRC passes after reassembly.

## Error injection

Test at minimum:

- [ ] missing middle fragment;
- [ ] reordered fragments;
- [ ] duplicate fragment;
- [ ] new packet sequence arriving before the old packet finishes;
- [ ] wrong payload-length field;
- [ ] corrupted canonical payload causing final CRC failure;
- [ ] disconnect during a partial packet;
- [ ] reconnect followed by a clean new packet.

## Payload-size cases

Validate at least:

- [ ] a small application payload that requires multiple fragments;
- [ ] the 20-byte application-payload example (5 fragments for 49 canonical bytes with the 8-byte fragment header);
- [ ] a payload large enough for a one-fragment canonical packet;
- [ ] the actual negotiated payload size used by the target phone/controller.

## Backpressure

- [ ] BLE-stack send failure/backpressure does not block DRDY handling;
- [ ] outbound queue is bounded;
- [ ] overload policy is explicit;
- [ ] dropped packet/fragment counters are observable;
- [ ] reconnect clears stale partial state.

## Throughput record

For every claimed working configuration, record:

- peripheral MCU/radio;
- firmware revision;
- phone/tablet model;
- OS version;
- BLE application payload size;
- connection parameters used by the platform;
- sample rate;
- ADS1299 device count;
- packets/fragments per second;
- duration of soak test;
- packet loss/reassembly/CRC counters.

Do not publish a generic BLE throughput claim without these measurements.

## 64-channel qualification

- [ ] each ADS1299 source has stable identity;
- [ ] eight packets per synchronized epoch are accounted for;
- [ ] fragment workload is calculated using the actual negotiated payload;
- [ ] phone/controller sustained throughput is measured at the intended rate;
- [ ] RF interference/reconnect behavior is tested;
- [ ] slow-client or radio stalls cannot grow memory without bound.

Passing this checklist validates the BLE transport path only. It does not validate ADS1299 analog performance, electrical isolation, EMC or clinical/medical safety.
