# Smart-glasses / XR platform adapter boundary

The generic integration core in this directory is intentionally platform-neutral.

Implemented generic layers:

```text
visual_stimulus_sync/
eye_tracking_sync/
xr_neurofeedback_ui/
```

## When to add a device-specific subproject

Add a platform/device subproject only when all of the following are known:

1. exact hardware/device family;
2. exact OS/runtime version;
3. documented SDK/API path;
4. transport path from ADS1299 or gateway;
5. runnable minimal application path;
6. timing/data capabilities actually exposed by the platform;
7. a validation plan on real hardware.

Do not add empty folders named after products solely for marketing coverage.

## Adapter responsibilities

A platform adapter may implement:

- BLE/TCP/WebSocket/USB connection to an ADS1299 gateway/device;
- rendering/presentation callbacks;
- eye-tracking API conversion into the vendor-neutral sample model;
- overlay state rendering;
- user interaction/command acknowledgement;
- platform lifecycle/reconnect handling.

It should not duplicate:

- canonical 49-byte packet semantics;
- EEG device sequence/timestamp definitions;
- generic multimodal clock-mapping logic;
- experiment event schema;
- research feature definitions.

## Android-based glasses / Android XR-class

Reuse the mobile Android protocol layer where practical:

```text
integrations/mobile/android/
```

Then place glasses-specific rendering, permissions, eye-tracking and lifecycle code in the future device/runtime adapter.

## visionOS-class / Apple XR

Reuse the Swift canonical protocol principles under:

```text
integrations/mobile/ios_ipados/
```

Platform-specific spatial rendering, device connectivity and any eye-tracking capability must follow the actual public API exposed on the tested device/runtime.

## Browser/WebXR-style path

When a browser runtime is appropriate, reuse the gateway/PWA message contract:

```text
integrations/open_edge_systems/linux_gateway/
integrations/mobile/web_pwa/
integrations/protocols/websocket/
```

Actual sensor/device access still depends on the browser/platform capabilities and must be tested.

## Validation evidence for a support claim

A future support matrix entry should include:

- hardware model;
- OS/runtime version;
- SDK/API version;
- transport;
- sample rate/channel count;
- visual timing method/result if stimuli are used;
- eye-tracking timing method/result if gaze is used;
- sustained run duration;
- known limitations.

Until that evidence exists, describe the platform as an architecture target, not as validated direct support.
