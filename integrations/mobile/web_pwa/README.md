# ADS1299-Core Web/PWA viewer

This is the first runnable user-interface project under `integrations/mobile/`.

It receives WebSocket JSON from the ADS1299-Core integration gateway and displays:

- eight live EEG/biopotential channels;
- sequence number;
- timestamp;
- packet count;
- sequence-gap count;
- ADS1299 status word.

Because it is browser-based, the same UI can be used during development on **Android, iPhone/iPad, Windows, macOS, Linux, ChromeOS and many smart-display/XR browsers** without rewriting the visualization layer.

## 1. Start the gateway

From `integrations/open_edge_systems/linux_gateway/`:

```bash
python -m pip install -r requirements.txt
python gateway.py --simulate
```

This produces a synthetic 8-channel stream at:

```text
ws://127.0.0.1:8765
```

## 2. Start a local web server

From this directory:

```bash
python -m http.server 8080
```

Open:

```text
http://127.0.0.1:8080
```

Press **Connect**.

## 3. Use a phone/tablet on the same development LAN

Run the gateway with a LAN binding:

```bash
python gateway.py --simulate --host 0.0.0.0
```

Serve the web app from a machine reachable by the phone, then set the viewer URL to:

```text
ws://<gateway-LAN-IP>:8765
```

The basic development gateway has no TLS/authentication, so keep this on a trusted local network.

## PWA behavior

The project includes a Web App Manifest and service worker so compatible browsers can install/cache the application shell. Live EEG data still requires a reachable ADS1299-Core gateway.

## Why start with a PWA

The PWA is not a replacement for future native Android/iOS apps. It is a fast, cross-platform reference client that validates:

- the WebSocket transport;
- the JSON integration schema;
- live plotting;
- sequence-gap detection;
- phone/tablet/desktop browser compatibility.

Native applications can later implement the same logical message model with device-specific BLE, background acquisition and file APIs.

## Validation status

- Source: implemented
- Synthetic gateway compatibility: designed for immediate testing
- Browser-device matrix: pending systematic validation
- Real ADS1299 stream: pending hardware validation
- Native Android/iOS integration: separate roadmap
