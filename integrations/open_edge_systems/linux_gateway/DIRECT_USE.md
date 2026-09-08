# Direct use — Linux edge gateway

This directory can be used as the host/edge bridge between ADS1299 acquisition hardware and browser/mobile/desktop clients.

## 1. Install dependencies

From this directory:

```bash
python -m pip install -r requirements.txt
```

The existing gateway uses `pyserial` and `websockets` only.

## 2. Validate without hardware

Run the existing simulated gateway:

```bash
python gateway.py --simulate
```

Default endpoint:

```text
ws://127.0.0.1:8765
```

Then open the repository PWA in:

```text
integrations/mobile/web_pwa/
```

For LAN development only, explicitly bind to an interface reachable by the client:

```bash
python gateway.py --simulate --host 0.0.0.0
```

Do not expose an unauthenticated development WebSocket directly to an untrusted network.

## 3. Use a serial device

Example:

```bash
python gateway.py --serial /dev/ttyACM0 --baud 921600
```

Windows-style device names such as `COM5` are also accepted by the existing Python gateway when run on Windows, although this folder is primarily the Linux/edge integration entry point.

The current `gateway.py` uses the repository `pc_tools/python/serial_reader.py` path. The newly added `canonical_packet.py` provides a dependency-light local decoder for new edge services that need to avoid that cross-directory dependency.

## 4. Validate the new edge core

```bash
python test_gateway_core.py
```

This checks:

- canonical 49-byte CRC decoding;
- arbitrary byte-chunk recovery;
- resynchronization after corruption;
- eight-device out-of-order arrival;
- 64-channel epoch assembly;
- bounded pending epochs;
- duplicate-device packet accounting.

## 5. Single-device architecture

```text
ADS1299/MCU
   |
USB/UART/TCP
   |
transport deframing
   |
49-byte canonical packet validation
   |
packet_message(...)
   |
WebSocket JSON / recording / LSL / analysis
```

Never perform socket broadcasting in an ADS1299 DRDY ISR. Acquisition hardware must already decouple DRDY handling from host transport using a bounded queue/ring.

## 6. Eight-device / 64-channel architecture

Each ADS1299 should keep an explicit stable `device_index` in the host integration configuration. Feed packets into:

```python
assembler = MultiDeviceEpochAssembler(device_count=8)
epoch = assembler.push(device_index, packet)
```

Only when all eight devices have produced the exact same sequence number does the assembler emit a 64-channel epoch.

The global channel mapping is:

```text
global_channel = device_index * 8 + local_channel
```

Do not use USB/TCP/Wi-Fi arrival order as a substitute for device identity or sample synchronization.

## 7. Timestamp handling

Preserve device timestamps. For a multi-device epoch, `MultiDeviceEpoch` retains one timestamp per device so synchronization quality can be inspected instead of silently replacing all device times with host receive time.

Host receive time may be added as metadata by an application, but should not overwrite the device timestamp.

## 8. Backpressure

A gateway can stall because of a slow WebSocket client, storage device, network link or downstream analysis process. Production integrations should:

- keep acquisition input and client output as separate tasks;
- use bounded queues;
- define an explicit overload/drop policy;
- expose dropped/gap counters;
- disconnect chronically slow clients instead of allowing unbounded memory growth.

The provided `MultiDeviceEpochAssembler` already bounds incomplete multi-device epochs, but it is not a complete outbound-client queue.

## 9. Security boundary

The existing development gateway is deliberately simple. Before deployment outside a trusted development LAN, add appropriate authentication, authorization, TLS termination, origin policy, logging and update management according to the deployment environment.

## 10. What this does not prove

Successful gateway operation does not prove:

- ADS1299 analog performance;
- electrical isolation or subject safety;
- EMC compliance;
- clinical/medical-device compliance;
- 64-channel hardware synchronization.

Those must be validated separately in the hardware and firmware layers.
