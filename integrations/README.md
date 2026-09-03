# ADS1299 ecosystem integrations

This directory is the bridge from **ADS1299 acquisition hardware** to the wider device and operating-system ecosystem.

The goal is not to make ADS1299 data stop at a serial terminal. The long-term target is a reusable path from an ADS1299 front end to **phones, tablets, desktops, smart watches, smart earbuds, smart glasses, XR devices, smart-home hubs, edge computers and open systems**.

> **Important:** a category appearing here does not automatically mean it is already production-ready. Every integration is labeled by status so roadmap breadth is never confused with verified support.

## Choose your destination

| I want ADS1299 data on... | Go here |
|---|---|
| Android / iPhone / iPad / HarmonyOS phone or tablet | [`mobile/`](mobile/) |
| Windows / macOS / Linux / ChromeOS computer | [`desktop_os/`](desktop_os/) |
| Apple Watch / Wear OS / Garmin / Zepp / other watch | [`smart_watches/`](smart_watches/) |
| Smart earbuds / headphones / ear-EEG wearable | [`smart_audio/`](smart_audio/) |
| Smart glasses / AR / XR headset | [`smart_glasses_xr/`](smart_glasses_xr/) |
| Home Assistant / openHAB / Node-RED / smart-home hub | [`smart_home_hubs/`](smart_home_hubs/) |
| Linux SBC / OpenWrt / Zephyr / open edge system | [`open_edge_systems/`](open_edge_systems/) |
| BLE / USB / TCP / WebSocket / MQTT / LSL / OSC transport | [`protocols/`](protocols/) |

## Architecture

```text
ADS1299 / multi-ADS1299
          │
          ▼
MCU / FPGA acquisition
          │
          ▼
Common ADS1299-Core packet model
          │
          ├──────── USB / UART ────────► PC / Linux / macOS / Windows
          │
          ├──────── BLE ───────────────► Android / iOS / Wearables
          │
          ├──────── Wi-Fi / TCP ───────► Phones / Web / Edge / Home hubs
          │
          ├──────── WebSocket / MQTT ──► Dashboards / Smart home / Cloud bridges
          │
          ├──────── LSL / OSC ─────────► EEG / BCI research software
          │
          └──────── device SDK bridges ► Watches / Earbuds / Glasses / XR
```

The **device-independent packet and signal model** is the key design principle. A transport adapter should not redefine what an ADS1299 sample means. Sequence counters, channel ordering, timestamps, integrity flags and metadata should remain consistent as data moves between systems.

## Integration families

### Mobile and tablet

Planned integration families include:

- Android
- iOS / iPadOS
- HarmonyOS / OpenHarmony
- browser-based / PWA viewers where appropriate

Typical functions:

- device discovery and connection;
- live 8/16/32/64-channel display;
- acquisition start/stop;
- signal-quality indicators;
- event markers;
- local recording;
- export to research formats;
- optional BCI inference or neurofeedback display.

### Desktop operating systems

The desktop track targets:

- Windows
- macOS
- Linux
- ChromeOS / Linux-container workflows where practical

Desktop applications can provide higher-channel-count visualization, long recording, debugging, firmware tools and research integration.

### Smart watches and wearables

The roadmap includes bridges for ecosystems such as:

- watchOS
- Wear OS
- HarmonyOS / OpenHarmony wearables
- Garmin Connect IQ class devices
- Zepp OS class devices

A watch may act as a display/controller, event-marker source, companion sensor or lightweight neurofeedback endpoint rather than directly hosting the ADS1299 acquisition driver.

### Smart earbuds and headphones

This track is especially relevant to **ear-EEG** and wearable neurotechnology. The integration layer will distinguish between:

- an ADS1299 acquisition module located in or near the wearable;
- a companion MCU that transports EEG data;
- a consumer earbud/headphone acting only as a display/audio-feedback endpoint;
- synchronization with audio stimuli for ERP/BCI experiments.

### Smart glasses and XR

The roadmap includes Android-based smart glasses, Android XR-class devices, visionOS-class devices and other AR/XR systems.

Potential uses include:

- visual stimulus presentation;
- SSVEP / P300 experiments;
- eye-tracking + EEG synchronization;
- neurofeedback overlays;
- hands-free BCI control;
- multimodal research interfaces.

### Smart home and ambient computing

ADS1299/BCI output can be integrated with automation systems through controlled gateways rather than wiring medical/research data directly into arbitrary devices.

Targets include:

- Home Assistant
- openHAB
- Node-RED
- MQTT bridges
- Matter-compatible application bridges where appropriate

Example research/prototyping use cases include neurofeedback-triggered scenes, accessibility interfaces and BCI-controlled home commands.

### Open and edge systems

Open-system support is intended for:

- Linux SBCs
- Raspberry Pi-class computers
- OpenWrt gateways
- Zephyr-based gateways
- ROS 2 / robotics bridges where useful
- containerized acquisition services

## Protocol strategy

The integration layer should prefer a small number of stable transports instead of inventing a separate protocol for every device category.

Priority transports:

1. USB CDC / serial
2. BLE GATT
3. TCP / UDP
4. WebSocket
5. MQTT
6. Lab Streaming Layer (LSL)
7. OSC where useful for interactive media / BCI prototyping

Each transport should document latency, throughput, packet-loss behavior, reconnect behavior and the maximum practical channel/sample-rate combination.

## Status vocabulary

- **Planned** — architecture and target defined; implementation not yet complete.
- **Prototype** — code exists and can be exercised, but interoperability is not fully validated.
- **Compiles** — reproducible build is verified.
- **Device-tested** — tested on at least one real target device/system.
- **Long-run tested** — sustained acquisition/streaming evidence exists.

## Contribution rule

A new integration should include more than an empty directory. It should eventually provide:

- target OS/device and tested version;
- connection/transport method;
- complete runnable application or bridge;
- expected input/output;
- packet mapping;
- reconnect/error handling;
- build/run instructions;
- screenshots or logs when available;
- latency/throughput notes;
- validation status.

The repository aims for **breadth with engineering credibility**: many destinations, one coherent ADS1299 data model, and explicit evidence for what has actually been tested.
