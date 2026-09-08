# Mobile PWA Deployment

The existing `web_pwa` viewer is intentionally browser-based so the same visualization can be used on Android, iPhone/iPad and desktop browsers during development.

## Development LAN workflow

1. Run the ADS1299 integration gateway on a trusted LAN.
2. Serve this PWA over HTTP/HTTPS from a host reachable by the phone/tablet.
3. Open the PWA URL in the mobile browser.
4. Enter the gateway WebSocket URL.
5. Verify sequence count, timestamp, status and live channels.

A local development example may use `ws://`, but production/deployed use should prefer a secure origin and `wss://` when traffic leaves a controlled development environment.

## PWA installability

The folder already contains:

- `manifest.webmanifest`
- `service-worker.js`

Install behavior varies by browser/OS. Do not claim identical background behavior or hardware access across Android and iOS.

## Background acquisition limitation

Mobile browsers may suspend timers, WebSockets or background pages when:

- the screen locks;
- the user switches apps;
- battery-saving policies activate;
- the OS reclaims resources.

Therefore the PWA should be treated primarily as a live viewer/reference client unless long-duration background behavior has been systematically validated on the target device/OS.

For a production mobile recorder, a native Android/iOS application may be more appropriate when reliable background acquisition, BLE device access or local file management is required.

## Network topology

Recommended development architecture:

`ADS1299 hardware -> USB/Ethernet/Wi-Fi -> gateway -> WebSocket JSON -> PWA`

The gateway owns binary transport parsing and canonical CRC validation. The browser receives already-validated logical EEG messages.

## 64-channel strategy

The current UI displays eight channels. For 64-channel use, do not simply increase the array length without defining:

- device identity;
- global channel order;
- sample grouping;
- rendering/downsampling strategy;
- mobile CPU/GPU load;
- WebSocket JSON bandwidth;
- recording strategy.

A mobile viewer can display selected channel groups while the gateway/desktop recorder preserves the complete raw 64-channel stream.

## Rendering performance

For sustained mobile performance:

- keep a bounded display history;
- avoid retaining all samples in browser memory;
- decouple display update rate from acquisition sample rate;
- downsample only for visualization, not for the authoritative raw recorder;
- report sequence gaps rather than hiding them.

The existing app uses a bounded history for eight channels.

## Offline shell vs live data

The service worker can cache the application shell. Offline installation does not mean live EEG is available offline; the gateway still must be reachable unless a future file-replay/local-device source is implemented.

## Validation matrix

For every claimed mobile target, record:

- device model;
- OS version;
- browser/version;
- installed-PWA vs browser-tab mode;
- Wi-Fi/network topology;
- gateway version/commit;
- sample rate/channel count;
- session duration;
- WebSocket reconnects;
- sequence gaps;
- rendering lag;
- behavior when screen locks/backgrounds.

Only configurations actually exercised should be labeled tested.
