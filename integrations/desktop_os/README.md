# Desktop operating-system integrations

Target systems: **Windows, macOS, Linux, and ChromeOS/Linux-container workflows where practical**.

The desktop layer is intended for debugging, long-duration recording, high-channel-count visualization, firmware interaction and research-tool interoperability.

## Common requirements

- serial/USB device discovery;
- optional TCP/WebSocket network acquisition;
- 8/16/32/64-channel visualization;
- packet integrity and dropped-frame reporting;
- raw recording and CSV/export helpers;
- metadata/event markers;
- connection diagnostics;
- hooks into Python/MATLAB/R and established EEG/BCI software.

## Target applications

The repository should converge on reusable protocol libraries plus thin platform-specific packaging rather than four completely unrelated applications.

Planned destinations include:

- Windows desktop acquisition/viewer;
- macOS acquisition/viewer;
- Linux GUI/CLI acquisition tools;
- ChromeOS-compatible workflows where device access permits.

Cross-platform technologies can be used when they reduce duplication without sacrificing reliable device access or performance.
