# UDP validation checklist

## Datagram framing

- [ ] one UDP datagram contains exactly one complete A9ET block;
- [ ] magic/version/count/payload length are validated;
- [ ] datagram length equals declared block size;
- [ ] every canonical packet passes sync/version/CRC;
- [ ] malformed datagrams are dropped without corrupting later state.

## Loss/reorder/duplicate injection

Test at minimum:

- [ ] one dropped datagram;
- [ ] several consecutive dropped datagrams;
- [ ] two datagrams delivered out of order;
- [ ] duplicated datagram;
- [ ] truncated datagram;
- [ ] datagram with valid A9ET header but bad canonical CRC;
- [ ] receiver queue overflow under burst traffic.

Verify the application reports these conditions rather than silently producing a continuous-looking EEG stream.

## Sequence tracking

- [ ] block-sequence gaps are counted;
- [ ] canonical packet-sequence gaps are counted;
- [ ] duplicate/reordered blocks are distinguishable from forward loss;
- [ ] uint32 wrap-around is handled deliberately;
- [ ] reconnect/restart behavior is documented if the sender process restarts.

## 64-channel qualification

- [ ] 8-packet block size is 408 bytes;
- [ ] global device/channel mapping is deterministic;
- [ ] all eight packets belong to the intended shared sample epoch;
- [ ] loss of one datagram creates one explicit epoch discontinuity;
- [ ] no later epoch is used to silently fill the missing one;
- [ ] sustained datagram rate is measured at the intended sample rate.

## Network qualification record

Record:

- sender/receiver hardware;
- OS/network stack;
- wired or wireless interface;
- switch/AP/router path where relevant;
- sample rate and ADS1299 count;
- datagrams/s and bytes/s;
- test duration;
- dropped/reordered/duplicate counts;
- host receive queue or application overflow counts;
- CPU/memory usage where relevant.

Passing this checklist validates UDP transport behavior only. It does not validate analog performance, electrical isolation, EMC or medical-device safety.
