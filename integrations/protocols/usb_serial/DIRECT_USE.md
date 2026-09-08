# Direct use — USB CDC / UART serial

## USB/raw canonical stream

When the device exposes an ordered USB byte stream containing canonical packets, use a stream parser that searches for sync and validates the full 49-byte packet.

Example host path:

```bash
python integrations/desktop_os/ads1299_desktop_capture.py serial <PORT> --baud 921600 --raw capture.bin --csv capture.csv
```

Use the actual port name and transport configuration for the target system. USB CDC drivers may expose a baud setting even when the physical USB transfer itself is not clocked by that baud value; firmware/driver behavior is platform-specific.

For binary-file replay:

```bash
python integrations/desktop_os/ads1299_desktop_capture.py file capture.bin --csv replay.csv
```

## UART/COBS stream

For the repository UART transport, use the COBS-aware host path rather than the raw canonical parser.

Wire framing:

```text
COBS(packet_49_bytes) + 0x00
```

Receiver sequence:

1. accumulate bytes until `0x00`;
2. COBS-decode the frame;
3. require exactly 49 decoded bytes;
4. validate sync/version/CRC;
5. decode sequence/timestamp/status/channels.

## Throughput budgeting

For one ADS1299 device, canonical payload rate before USB/UART framing is:

```text
49 bytes/sample * sample_rate
```

For N ADS1299 devices:

```text
49 * sample_rate * N bytes/s
```

For UART 8N1, a rough lower-bound serial line budget is:

```text
wire_bits/s >= encoded_bytes/s * 10
```

COBS adds small framing overhead, and engineering margin is still required. Do not choose a baud rate that only matches the theoretical minimum.

For USB, also account for endpoint packetization, host scheduling, buffering and application latency.

## 64-channel path

For 8 devices:

```text
8 packets/epoch * 49 bytes = 392 canonical bytes/epoch
```

At higher sample rates, prefer batching and asynchronous transfers where supported. Keep acquisition and host-output buffering separate from DRDY handling.

## Recovery behavior

Raw canonical stream receivers should resynchronize after corruption by searching for the next valid sync+length+CRC candidate.

UART receivers should resynchronize at the next `0x00` delimiter, COBS-decode, then rely on canonical CRC for final integrity.

## Do not assume

Do not assume:

- one host read equals one packet;
- one USB transfer equals one sample epoch;
- a serial device name is stable across hosts;
- a configured baud rate proves sustainable throughput;
- a successful short recording proves long-duration reliability.
