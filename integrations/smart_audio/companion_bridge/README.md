# Smart-audio companion bridge

This directory defines a safe integration boundary for systems where the ADS1299 acquisition path and the audio endpoint are controlled by a phone, PC, edge computer or other open companion controller.

This is the preferred approach when a consumer earbud/headphone does not expose a documented public low-level SDK/API suitable for direct integration.

## Architecture

```text
ADS1299 acquisition --------------------+
                                        |
                                        v
                                companion controller
                                        |
                         +--------------+--------------+
                         |                             |
                         v                             v
                recording / analysis           audio endpoint API
                                                (only if documented)
```

The companion controller owns coordination, but it does not become the source of truth for ADS1299 acquisition timing.

## Adapter boundary

Every audio endpoint adapter should expose only the capabilities it can actually support, for example:

- connect/disconnect;
- list/select a documented output endpoint;
- start/stop a known stimulus;
- report software dispatch/result state;
- expose device/API timing information when the platform truly provides it.

Do not fabricate:

- acoustic onset timestamps;
- battery state;
- sensor access;
- low-level earbud firmware control;
- EEG-from-earbud capability;
- hidden proprietary telemetry.

## Acquisition boundary

ADS1299 samples remain in the canonical repository data path. The bridge may consume validated EEG packets/events, but it should not convert the audio endpoint into an implicit EEG clock source.

## Vendor/platform adapters

Put vendor-specific code behind a small adapter interface. Keep the experiment controller independent of a specific brand/API so unsupported endpoints can be replaced without rewriting EEG protocol logic.

An adapter should document:

- exact SDK/API used;
- OS/platform version;
- supported commands;
- timing information actually available;
- reconnect behavior;
- validation hardware.

## Security/privacy

A companion app may handle EEG streams, event labels and network/audio device identifiers. Minimize stored identifiers, keep sensitive participant metadata out of device/topic names, and define access controls for network-facing bridges.
