# Mobile and tablet integrations

Target systems: **Android, iOS, iPadOS, HarmonyOS / OpenHarmony, and browser/PWA clients where appropriate**.

## First runnable integration

[`web_pwa/`](web_pwa/) is the first executable cross-platform client in this category. It consumes the ADS1299-Core WebSocket gateway and provides live 8-channel plotting, sequence tracking and status display in a browser/PWA.

This gives Android, iPhone/iPad, Windows, macOS, Linux and ChromeOS-class browsers a common reference client while native applications are developed separately.

## Mature application baseline

Every mature mobile integration should support:

- device discovery and reconnect;
- acquisition start/stop;
- 8-channel live waveform display first, then scalable 16/32/64-channel views;
- sequence/CRC/integrity monitoring;
- signal-quality indicators;
- event markers;
- local recording and export;
- configurable channel names and sample-rate metadata;
- optional EEG/BCI processing hooks.

## Preferred transports

- BLE GATT for portable low-power systems;
- Wi-Fi / TCP or WebSocket for higher throughput and development convenience;
- USB where the mobile device and hardware support a practical wired accessory path.

## Target subprojects

- `web_pwa/` — **implemented prototype**: cross-platform WebSocket viewer
- `android/` — planned native Android reference app
- `ios_ipados/` — planned Apple mobile reference app
- `harmonyos_openharmony/` — planned Huawei/OpenHarmony ecosystem reference

Subprojects are only added when there is enough implementation to be useful. Roadmap names alone stay documented here rather than becoming empty folders.
