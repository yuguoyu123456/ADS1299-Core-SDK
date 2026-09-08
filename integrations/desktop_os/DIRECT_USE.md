# Desktop Direct Use

This folder now contains reusable command-line tools for Windows, macOS and Linux.

## 1. Requirements

- Python 3.10+ recommended.
- No third-party package is required for file/stdin/TCP canonical-stream parsing.
- `pyserial` is required only for serial/USB CDC/UART access:

```bash
python -m pip install pyserial
```

## 2. Run the smoke test

From `integrations/desktop_os/`:

```bash
python test_desktop_capture.py
```

The test covers:

- 49-byte canonical packet decode;
- CRC16-CCITT-FALSE;
- signed channel values;
- arbitrary stream chunk boundaries;
- sync recovery after corruption;
- sequence-gap detection;
- COBS round trip.

This is a software smoke test. It does not prove a physical ADS1299 board or serial/USB device has been validated.

## 3. Replay a saved canonical byte stream

```bash
python ads1299_desktop_capture.py file capture.bin --csv capture.csv
```

The parser searches for `0xA5 0x5A`, validates protocol version and CRC, and reports sequence gaps.

To preserve only validated canonical packets into a clean raw file:

```bash
python ads1299_desktop_capture.py file noisy_stream.bin --raw clean_packets.bin
```

## 4. TCP canonical stream

If the device/server exposes a raw stream of canonical 49-byte packets:

```bash
python ads1299_desktop_capture.py tcp 192.168.1.50 9000 --raw session.bin --csv session.csv
```

TCP read boundaries are arbitrary; the stream parser handles split/combined packets.

If the sender uses the repository Ethernet transport block format instead of a raw canonical stream, decode the Ethernet blocks first. Do not feed an Ethernet block header directly to the canonical-only parser and expect it to represent one packet.

## 5. USB CDC or raw serial canonical stream

When firmware sends canonical packet bytes directly over a serial-like USB CDC endpoint:

Windows example:

```bash
python ads1299_desktop_capture.py serial COM5 --baud 921600 --raw eeg.bin --csv eeg.csv
```

macOS/Linux example:

```bash
python ads1299_desktop_capture.py serial /dev/ttyACM0 --baud 921600 --raw eeg.bin --csv eeg.csv
```

Use the actual device port and configured baud rate. Do not assume `921600` is supported or sufficient for every target/channel count.

## 6. Repository UART transport: COBS + 0x00

`firmware/transport/uart/` uses COBS framing with a zero delimiter. Use the dedicated UART helper:

Windows:

```bash
python ads1299_uart_cobs_capture.py COM5 --baud 921600 --raw eeg.bin --csv eeg.csv
```

macOS/Linux:

```bash
python ads1299_uart_cobs_capture.py /dev/ttyUSB0 --baud 921600 --raw eeg.bin --csv eeg.csv
```

The helper:

1. waits for `0x00` frame delimiter;
2. COBS-decodes the frame;
3. requires exactly one valid canonical packet;
4. validates CRC;
5. tracks sequence gaps;
6. optionally saves raw canonical packets and CSV raw counts.

## 7. Counts vs microvolts

The generic recorder defaults to raw ADC counts.

To export microvolts:

```bash
python ads1299_desktop_capture.py file capture.bin --csv capture_uv.csv --units uv --vref 4.5 --gain 24
```

Only use Vref/gain values that match the real hardware and ADS1299 configuration. The example values above are CLI examples, not a declaration that every board uses those settings.

The UART COBS helper intentionally stores raw counts only so raw acquisition remains the primary recorded representation.

## 8. Recommended raw-recording policy

For engineering and research capture, keep:

- validated canonical binary packets;
- sequence/timestamp/status;
- raw channel counts;
- board/device metadata;
- sample rate;
- PGA gain;
- Vref/clock/reference configuration;
- channel/electrode map;
- packet-loss statistics.

Do not keep only a filtered CSV or screenshot.

## 9. 64-channel use

One 64-channel sample group contains eight canonical device packets. The desktop layer should group packets by the documented device/slot identity and sample epoch, not merely by network arrival order.

For high-rate 64-channel capture, prefer a transport with measured throughput margin such as USB or Ethernet. Validate queue and host disk-writing behavior during long recordings.

## 10. Exit status

`ads1299_desktop_capture.py` returns:

- `0` when the completed capture has no parser errors or missing sequence values;
- `1` for setup/I/O/runtime errors;
- `2` when data-integrity problems such as CRC/parser errors or missing packets were observed.

This makes the tool usable in automated validation scripts.
