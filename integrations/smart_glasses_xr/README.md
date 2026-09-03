# Smart glasses, AR and XR integrations

This category covers **Android-based smart glasses, Android XR-class devices, visionOS-class devices and other AR/XR platforms**.

Potential roles include:

- visual stimulus presentation for SSVEP/P300 research;
- multimodal EEG + eye-tracking experiments;
- neurofeedback overlays;
- hands-free BCI command interfaces;
- experiment prompts and event markers;
- accessibility prototypes;
- spatial UI for EEG/BCI monitoring.

## Integration architecture

```text
ADS1299 → MCU/FPGA → BLE/Wi-Fi/USB → glasses/XR app
                                  ├─ event markers
                                  ├─ stimulus timing
                                  ├─ EEG summaries / inference
                                  └─ optional raw data where bandwidth permits
```

## Timing matters

For ERP, SSVEP and multimodal research, a visually impressive app is not enough. Every serious XR integration should document:

- display/stimulus timing method;
- event-marker timestamp source;
- transport latency and jitter;
- synchronization with EEG samples;
- frame-rate assumptions;
- eye-tracking synchronization when present.

Device-specific subprojects should be added only when a runnable SDK path exists and the exact hardware/OS can be documented.
