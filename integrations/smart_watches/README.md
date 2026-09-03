# Smart-watch and wearable integrations

This category covers **watchOS, Wear OS, HarmonyOS/OpenHarmony wearables, Garmin-class devices, Zepp OS-class devices, and similar wearable platforms**.

A smart watch does not necessarily need to run the ADS1299 driver directly. In many systems it is more useful as a:

- remote start/stop controller;
- event-marker source;
- signal-quality display;
- neurofeedback endpoint;
- experiment prompt/stimulus companion;
- synchronized source of IMU/PPG/heart-rate metadata;
- notification and accessibility interface.

## Reference architecture

```text
ADS1299 → acquisition MCU → BLE/Wi-Fi → phone / gateway → watch
                                  └──── direct watch link where practical
```

## Validation requirements

A watch integration should document:

- exact device/model and OS version;
- transport path;
- achievable update rate;
- reconnect behavior;
- battery implications;
- latency for event markers or neurofeedback;
- which data stays on the watch and which is only summarized.

Raw 64-channel EEG should not be assumed to fit every wearable transport or UI. The integration must match the device's real bandwidth and role.
