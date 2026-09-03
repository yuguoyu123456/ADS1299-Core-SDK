# Mobile and tablet integrations

Target systems: **Android, iOS, iPadOS, HarmonyOS / OpenHarmony, and browser/PWA clients where appropriate**.

## Planned application baseline

Every mature mobile integration should support:

- device discovery and reconnect;
- acquisition start/stop;
- 8-channel live waveform display first, then scalable 16/32/64-channel views;
- sequence/CRC/integrity monitoring;
- signal-quality indicators;
- event markers;
- local recording and export;
- configurable channel names and sample rate metadata;
- optional EEG/BCI processing hooks.

## Preferred transports

- BLE GATT for portable low-power systems;
- Wi-Fi / TCP or WebSocket for higher throughput and development convenience;
- USB where the mobile device and hardware support a practical wired accessory path.

## Target subprojects

- `android/` — native Android reference app
- `ios_ipados/` — Apple mobile reference app
- `harmonyos_openharmony/` — Huawei/OpenHarmony ecosystem reference
- `web_pwa/` — browser/PWA viewer where browser transport APIs are suitable

Subprojects should only be added when there is enough implementation to be useful. Roadmap names alone are documented here rather than represented by empty folders.
