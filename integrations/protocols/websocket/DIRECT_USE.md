# Direct use — WebSocket

## Reference gateway

From:

```text
integrations/open_edge_systems/linux_gateway/
```

Install dependencies:

```bash
python -m pip install -r requirements.txt
```

Start simulated source:

```bash
python gateway.py --simulate
```

Default development endpoint:

```text
ws://127.0.0.1:8765
```

The browser/PWA client is under:

```text
integrations/mobile/web_pwa/
```

## Real serial source

Example:

```bash
python gateway.py --serial /dev/ttyACM0 --baud 921600
```

The gateway should receive already framed/validated ADS1299 data through the repository host tooling before exposing EEG JSON.

## Client behavior

A client should:

1. wait for/accept `hello` metadata;
2. dispatch messages by `type`;
3. validate `eeg.channels.length == 8`;
4. validate `eeg_epoch.channel_count` against the actual array length;
5. track sequence gaps independently;
6. treat reconnect as a potential discontinuity;
7. keep rendering/UI load from blocking message ingestion.

## Slow clients

A server must not allow one slow browser/device to cause unbounded buffering for all clients.

Production designs should use:

- bounded per-client queues or equivalent flow control;
- explicit drop/disconnect policy;
- observable dropped-message counters;
- separation between acquisition input and WebSocket broadcast tasks.

## 64-channel use

For 8×ADS1299, prefer one `eeg_epoch` object per synchronized epoch when the application wants a single 64-channel update.

Preserve per-device timestamps/status and deterministic channel mapping. Do not infer synchronization from JSON arrival order.

## JSON vs raw binary

WebSocket can technically carry binary frames, but this repository's implemented browser integration currently uses JSON. If a future binary WebSocket mode is added, it should reuse the canonical 49-byte packet or the existing A9ET block rather than introducing an incompatible sample format.

## Security boundary

For untrusted networks, add appropriate:

- TLS (`wss://`);
- authentication/authorization;
- origin/access policy;
- rate limiting;
- logging/monitoring;
- deployment secret management.

Do not treat a development `ws://` endpoint as a production security design.
