# Device and operating-system integration matrix

This matrix separates **ecosystem coverage** from **verified integration status**.

| Category | Target ecosystems | Current status | Intended role |
|---|---|---|---|
| Mobile | Android | Planned | BLE/Wi-Fi acquisition, live view, recording, control |
| Mobile | iOS / iPadOS | Planned | BLE/Wi-Fi acquisition, live view, recording, control |
| Mobile | HarmonyOS / OpenHarmony | Planned | portable acquisition and companion app |
| Desktop | Windows | PC tools exist; unified app roadmap | acquisition, debugging, recording, analysis bridge |
| Desktop | macOS | Planned integration packaging | acquisition, recording, analysis bridge |
| Desktop | Linux | Python/host tooling foundation; deeper integration planned | CLI/GUI, services, gateways |
| Desktop | ChromeOS | Planned | Linux/browser-compatible workflows where supported |
| Smart watch | watchOS | Planned | companion display/control/neurofeedback |
| Smart watch | Wear OS | Planned | companion display/control/neurofeedback |
| Smart watch | HarmonyOS/OpenHarmony wearable | Planned | companion display/control |
| Smart watch | Garmin-class | Planned | markers/feedback/companion functions |
| Smart watch | Zepp OS-class | Planned | markers/feedback/companion functions |
| Smart audio | Earbuds/headphones/ear-EEG | Planned | ear-EEG, auditory BCI, audio feedback and synchronization |
| Smart glasses/XR | Android-based smart glasses | Planned | stimuli, markers, EEG/eye-tracking, neurofeedback |
| Smart glasses/XR | Android XR-class | Planned | XR BCI / multimodal experiments |
| Smart glasses/XR | visionOS-class | Planned | XR BCI / multimodal experiments |
| Smart home | Home Assistant | Planned | explicit BCI/event bridge |
| Smart home | openHAB | Planned | explicit BCI/event bridge |
| Smart home | Node-RED | Planned | rapid prototyping / automation bridge |
| Smart home | MQTT / Matter-facing bridge | Planned | distributed integration |
| Open edge | Linux SBC | Planned first-class gateway | logging, network bridge, preprocessing |
| Open edge | OpenWrt | Planned | embedded network gateway |
| Open edge | Zephyr gateway | Planned | embedded/open gateway |
| Research transport | LSL | Planned priority | neuroscience/multimodal stream interoperability |
| Interactive transport | OSC | Planned | BCI/media prototyping |
| Network transport | TCP/UDP/WebSocket/MQTT | Planned common adapters | cross-device streaming |
| Portable transport | BLE GATT | Planned priority | phones/wearables |
| Wired transport | USB CDC / serial | Existing foundation | PC/edge acquisition |

## Status definitions

- **Planned** — target and architecture documented; no claim of a runnable integration.
- **Prototype** — runnable code exists; device/system interoperability is not yet fully validated.
- **Compiles** — clean reproducible build is confirmed.
- **Device-tested** — tested on at least one real target device/OS version.
- **Long-run tested** — sustained acquisition/streaming evidence is documented.

A target should move upward only when evidence exists. Broad roadmap coverage is valuable, but this repository does not convert roadmap names into false support claims.
