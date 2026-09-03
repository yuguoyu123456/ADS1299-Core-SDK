# ADS1299-Core serial-to-WebSocket gateway

This is the first executable foundation under `integrations/`.

It converts the repository's existing **49-byte ADS1299-Core serial packet** into a WebSocket JSON stream that can be consumed by browser apps, Android/iOS prototypes, smart-glass interfaces, dashboards and edge services.

The gateway runs on **Windows, macOS and Linux** as long as Python and the serial device are available.

## Install

From this directory:

```bash
python -m pip install -r requirements.txt
```

## Run without ADS1299 hardware

A simulator is included so the integration layer can be exercised immediately:

```bash
python gateway.py --simulate
```

Default endpoint:

```text
ws://127.0.0.1:8765
```

## Run with a real ADS1299-Core serial stream

Windows example:

```bash
python gateway.py --serial COM5 --baud 921600
```

Linux example:

```bash
python gateway.py --serial /dev/ttyACM0 --baud 921600
```

macOS example:

```bash
python gateway.py --serial /dev/cu.usbserial-XXXX --baud 921600
```

## JSON message

A typical message is:

```json
{
  "type": "eeg",
  "protocol_version": 0,
  "flags": 0,
  "sequence": 1250,
  "timestamp_us": 5000000,
  "status_hex": "C00000",
  "channels": [102, -51, 77, 20, -10, 8, 44, -32],
  "gateway": {
    "received_frames": 1251,
    "sequence_gaps": 0
  }
}
```

The binary packet remains the authoritative embedded transport. JSON is a convenient integration representation for higher-level devices.

## Network exposure

The default host is `127.0.0.1`, so the gateway is local-only.

To make it reachable from another device on a trusted development LAN:

```bash
python gateway.py --serial /dev/ttyACM0 --host 0.0.0.0
```

This basic development gateway does not implement authentication or TLS. Do not expose it directly to the public internet.

## Why this exists

Without a common gateway, every Android/iOS/watch/glasses prototype would need its own serial parser and packet-recovery logic. This bridge lets those clients begin from a stable WebSocket stream while the embedded packet protocol remains shared across STM32, ESP32, RP2040 and future MCU/FPGA projects.

## Validation status

- Source: implemented
- Synthetic stream: designed for immediate local testing
- CI/syntax test: to be tracked in the integration PR
- Real ADS1299 serial stream: pending hardware validation
- Long-run network test: pending
