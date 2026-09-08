# Linux gateway validation checklist

Use this checklist before treating a Linux edge gateway as a stable integration target.

## A. Dependency and startup validation

- [ ] `python -m pip install -r requirements.txt` succeeds in the intended environment.
- [ ] `python gateway.py --simulate` starts without exceptions.
- [ ] a WebSocket client can connect and receives a `hello` message.
- [ ] the existing PWA can receive the simulated stream.
- [ ] the gateway exits/restarts cleanly after interruption.

## B. Core protocol validation

Run:

```bash
python test_gateway_core.py
```

Expected result:

```text
linux gateway core smoke tests: PASS
```

Confirm separately that:

- [ ] valid 49-byte packets are accepted;
- [ ] wrong CRC packets are rejected;
- [ ] arbitrary USB/TCP chunk boundaries do not alter packet recovery;
- [ ] garbage bytes before sync are discarded;
- [ ] parser counters expose discarded/corrupt data.

## C. Real serial/USB input

- [ ] the selected device node/COM port is correct.
- [ ] configured baud rate matches firmware when using UART/CDC serial semantics.
- [ ] sustained stream does not show unexplained sequence gaps.
- [ ] disconnect/reconnect behavior is defined and tested.
- [ ] OS permissions/udev policy are handled for unattended Linux operation when needed.

Do not confuse repository UART COBS framing with a raw canonical USB/TCP stream. Deframe the transport before canonical decoding.

## D. WebSocket client validation

- [ ] `hello` is received once per connection.
- [ ] `eeg` messages contain exactly 8 channels.
- [ ] sequence/timestamp fields remain numeric and within uint32 range.
- [ ] malformed JSON does not crash clients.
- [ ] slow/disconnected clients do not cause unbounded gateway memory growth.
- [ ] status/diagnostic requests work as expected.

## E. 8×ADS1299 / 64-channel validation

- [ ] each physical ADS1299 has a stable explicit `device_index` 0..7.
- [ ] device identity is never inferred from packet arrival order.
- [ ] all eight devices are configured to use the intended synchronization strategy.
- [ ] matching sequence numbers truly refer to the same acquisition epoch.
- [ ] one completed epoch produces exactly 64 channels.
- [ ] mapping is verified as `global = device_index * 8 + local_channel`.
- [ ] per-device timestamps are retained and inspected.
- [ ] missing devices create incomplete/evicted epochs rather than silently shifted channels.
- [ ] bounded pending storage prevents memory growth during a stalled device.

## F. Throughput and soak testing

Test at the actual intended sample rate and channel count, not only with short simulations.

Record at least:

- input packets/s;
- output messages/s;
- sequence gaps;
- parser/CRC failures;
- incomplete multi-device epochs;
- queue high-water marks where applicable;
- CPU usage;
- memory usage;
- client disconnects;
- storage write latency if recording.

Run long enough to expose thermal throttling, Wi-Fi jitter, USB resets, filesystem latency and memory-growth bugs.

## G. Time behavior

- [ ] device `timestamp_us` is preserved.
- [ ] host receive time, if added, is stored as separate metadata.
- [ ] timestamp wrap-around is considered for long recordings.
- [ ] multi-device clock/START synchronization is validated in firmware/hardware, not inferred from host receive time.

## H. Security/deployment validation

For anything beyond a trusted development LAN:

- [ ] TLS termination or equivalent secure transport is defined.
- [ ] authentication/authorization policy is defined.
- [ ] allowed origins/clients are controlled where appropriate.
- [ ] debug endpoints are not unintentionally exposed.
- [ ] logs avoid leaking sensitive study/participant metadata.
- [ ] software update and rollback procedures are documented.

## I. Scientific and hardware boundary

Passing this checklist validates a software integration path only. It does not by itself validate:

- electrode safety;
- leakage current;
- electrical isolation;
- analog noise/CMRR;
- EMC;
- ADS1299 timing margins;
- 64-channel sampling simultaneity;
- medical-device or clinical compliance.

Use the repository hardware and firmware validation procedures for those layers.
