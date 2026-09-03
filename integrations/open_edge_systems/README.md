# Open and edge-system integrations

This category covers open, embeddable and gateway-style systems that can sit between ADS1299 acquisition hardware and higher-level applications.

Target environments include:

- Linux single-board computers;
- Raspberry Pi-class Linux systems;
- OpenWrt gateways;
- Zephyr-based gateways;
- containerized acquisition services;
- ROS 2 bridges where useful for robotics / multimodal research;
- other open operating systems with stable serial, USB, BLE or networking APIs.

## Typical roles

- headless data logger;
- USB-to-network bridge;
- BLE-to-TCP bridge;
- local signal-quality monitor;
- timestamp/event gateway;
- preprocessing or feature-extraction edge node;
- MQTT/WebSocket/LSL publisher;
- multi-sensor synchronization hub.

## Design principle

Keep the acquisition model independent of the gateway OS. The same ADS1299 packet semantics should survive transport through Linux, OpenWrt or another gateway without redefining channel order, timestamps or integrity flags.

## Validation checklist

- documented OS/distribution version;
- exact device/SBC used;
- startup/service configuration;
- reconnect behavior;
- sustained throughput;
- CPU and memory load;
- packet-loss statistics;
- timestamp behavior;
- long-run logging evidence.
