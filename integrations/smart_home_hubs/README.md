# Smart-home and ambient-computing integrations

This track connects ADS1299/BCI outputs to **smart-home controllers and ambient-computing systems** through explicit application bridges.

Planned ecosystems include:

- Home Assistant
- openHAB
- Node-RED
- MQTT-based automation
- Matter-facing application bridges where appropriate

## Example research/prototyping use cases

- BCI accessibility commands;
- neurofeedback-linked lighting/audio scenes;
- experiment-triggered environmental events;
- occupancy/context fusion with physiological signals;
- hands-free control demonstrations.

## Architecture

```text
ADS1299 → MCU/FPGA → PC/edge gateway → validated BCI/event layer → smart-home bridge
```

The home-automation layer should consume **explicit commands/events**, not raw EEG amplitudes as uncontrolled actuator commands.

## Safety and reliability

Integrations should document:

- command confirmation/debounce strategy;
- false-positive handling;
- timeout/failsafe behavior;
- network disconnect behavior;
- user override;
- which automations are demonstration-only.

The repository is for research, education and prototyping; a BCI demo is not a safety-certified home-control system.
