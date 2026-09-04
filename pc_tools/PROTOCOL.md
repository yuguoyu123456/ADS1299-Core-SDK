# ADS1299-Core host packet protocol (draft v0)

Goal: all MCU and FPGA examples emit the same host stream so Python/MATLAB/R tooling is reusable.

## Packet

Little-endian fields unless noted.

| Field | Bytes | Description |
|---|---:|---|
| Sync | 2 | `0xA5 0x5A` |
| Version | 1 | protocol version, initially `0x00` |
| Flags | 1 | transport/status flags |
| Sequence | 4 | monotonically increasing frame counter |
| Timestamp_us | 4 | optional modulo-2^32 microsecond timestamp |
| ADS status | 3 | raw ADS1299 status bytes |
| CH1..CH8 | 32 | eight signed int32 values, sign-extended from 24 bit |
| CRC16 | 2 | CRC-16/IBM-3740 over Version through CH8, transmitted little-endian |

Total: 49 bytes.

## CRC definition

The checksum historically exposed by the code as `crc16_ccitt` is the fully specified CRC-16/IBM-3740 parameter set, commonly also called CRC-16/CCITT-FALSE:

- width: 16
- polynomial: `0x1021`
- initial value: `0xFFFF`
- input/output reflection: false
- xorout: `0x0000`
- canonical check: ASCII `123456789` -> `0x29B1`

Naming the parameters avoids ambiguity because several incompatible CRC-16 algorithms are called "CCITT" in software and documentation.

## Interoperability vector

The repository keeps one byte-exact vector in both the C and Python regression suites. Inputs are:

- Version `0`, flags `3`
- Sequence `123456`
- Timestamp `987654`
- Status `C0 00 00`
- Channels `0, 1, -1, 8388607, -8388608, 123, -456, 42`

Expected 49-byte packet, in hexadecimal:

```text
a55a000340e2010006120f00c000000000000001000000ffffffffffff7f00000080ff7b00000038feffff2a0000007790
```

The final two bytes `77 90` are the little-endian representation of CRC `0x9077` for this packet body.

## Design rules

- MCU/FPGA code MUST preserve the raw ADS1299 status bytes.
- Channel samples are transmitted as signed 32-bit values to simplify host parsers.
- The sequence number allows packet-loss detection.
- Transport framing is separate from EEG filtering/BCI processing.
- C and Python implementations must pass the shared golden vector before protocol compatibility is claimed.
- This format remains a draft until broader transport and implementation interoperability tests are completed.
