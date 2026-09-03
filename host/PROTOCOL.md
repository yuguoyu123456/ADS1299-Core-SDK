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
| CRC16-CCITT | 2 | over Version through CH8 |

Total: 49 bytes.

## Design rules

- MCU/FPGA code MUST preserve the raw ADS1299 status bytes.
- Channel samples are transmitted as signed 32-bit values to simplify host parsers.
- The sequence number allows packet-loss detection.
- Transport framing is separate from EEG filtering/BCI processing.
- This format is a draft until interoperability tests are completed.
