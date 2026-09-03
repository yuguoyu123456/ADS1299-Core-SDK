# Cross-platform transport and interoperability protocols

The integration ecosystem should use a small set of well-understood transports instead of inventing a unique wire format for every app or device class.

## Priority transports

### USB CDC / serial
Best for development, debugging, desktop tools and deterministic wired acquisition.

### BLE GATT
Best for portable battery-powered systems, phones and wearables. Every BLE profile must state practical throughput limits for the chosen connection parameters and channel/sample-rate combination.

### TCP
Best for reliable higher-throughput network streaming when retransmission and ordered delivery are acceptable.

### UDP
Useful for low-latency controlled-network experiments when the application explicitly handles packet loss and ordering.

### WebSocket
Useful for browser/PWA dashboards and cross-platform network viewers.

### MQTT
Useful for telemetry, commands, smart-home gateways and distributed system integration. Raw high-rate EEG should only be sent through MQTT when the broker/network design is deliberately sized for it.

### Lab Streaming Layer (LSL)
A priority interoperability target for neuroscience and multimodal laboratory acquisition because it provides a common research-stream interface and timing model.

### OSC
Useful for interactive media, rapid BCI prototyping and creative/experimental applications.

## Common payload principle

Transport-specific framing may differ, but applications should preserve the same logical fields wherever practical:

- protocol version;
- device/system identifier;
- sequence number;
- timestamp;
- status word;
- channel count;
- signed channel samples;
- sample rate;
- channel metadata;
- integrity/error flags;
- optional event markers.

## Every transport adapter must document

1. maximum tested throughput;
2. tested channel count and sample rate;
3. expected latency;
4. reconnect strategy;
5. dropped/reordered packet behavior;
6. MTU/frame-size assumptions;
7. integrity checking;
8. exact hardware/OS used for validation.

The goal is **one ADS1299 data model, many reliable transports**.
