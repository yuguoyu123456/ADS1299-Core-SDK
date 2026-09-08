# ADS1299 common data packet

This module converts the native ADS1299 27-byte sample frame (`STATUS[3] + 8 x 24-bit channels`) into a transport-friendly fixed 49-byte packet.

## What is provided

- 24-bit two's-complement sign extension for ADS1299 samples.
- Complete raw-frame decoder for one ADS1299 device.
- Fixed-size packet encoder and decoder.
- CRC-16/CCITT-FALSE integrity check.
- Sequence counter and microsecond timestamp fields.
- Flags for valid data, lead-off, internal test signal, clock source, daisy-chain state and overflow.
- ADC-code to volt / microvolt conversion helpers.
- C/C++ compatible public header with no dynamic allocation.

## 49-byte wire format

| Offset | Length | Field | Encoding |
|---:|---:|---|---|
| 0 | 2 | Sync | `A5 5A` |
| 2 | 1 | Version | currently `0x00` |
| 3 | 1 | Flags | bit mask |
| 4 | 4 | Sequence | little-endian `uint32_t` |
| 8 | 4 | Timestamp | little-endian microseconds |
| 12 | 3 | ADS1299 STATUS | same byte order as SPI output |
| 15 | 32 | CH1..CH8 | eight sign-extended `int32_t`, little-endian |
| 47 | 2 | CRC16 | little-endian CRC over bytes 2..46 |

The fixed packet size intentionally avoids packed C structs on the wire so that MCU, FPGA and Linux implementations stay ABI-independent.

## Minimal acquisition path

```c
#include "ads1299_packet.h"

uint8_t ads_raw[27];       /* fill from one RDATA/RDATAC SPI transfer */
uint8_t status[3];
int32_t ch[8];
uint8_t tx[ADS1299_PACKET_SIZE];

if (ads1299_decode_raw_frame(ads_raw, sizeof(ads_raw), status, ch)) {
    size_t n = ads1299_packet_encode(
        tx,
        sizeof(tx),
        ADS1299_PACKET_FLAG_DATA_VALID | ADS1299_PACKET_FLAG_TIMESTAMP_VALID,
        sample_sequence,
        timestamp_us,
        status,
        ch);

    if (n == ADS1299_PACKET_SIZE) {
        transport_write(tx, n);
    }
}
```

## Receiver path

```c
ads1299_packet_t packet;
ads1299_packet_result_t rc = ads1299_packet_decode(rx, rx_len, &packet);
if (rc == ADS1299_PACKET_OK) {
    for (unsigned i = 0; i < ADS1299_PACKET_CHANNELS; ++i) {
        double uv = ads1299_code_to_microvolts(packet.channels[i], 4.5, 24);
        consume_channel(i, uv);
    }
}
```

## ADS1299 raw sample notes

Each ADS1299 channel is a signed 24-bit two's-complement value. The helper `ads1299_decode_sample24_be()` accepts the three bytes exactly in the order read from SPI and sign-extends them into `int32_t`.

For one device in normal 8-channel mode, one sample frame is 27 bytes:

```text
STATUS0 STATUS1 STATUS2
CH1_MSB CH1_MID CH1_LSB
...
CH8_MSB CH8_MID CH8_LSB
```

For multiple ADS1299 devices in daisy-chain mode, decode each device's 27-byte section separately and then either send one common packet per device or build a higher-level multi-device frame in the transport/application layer.

## Error handling

`ads1299_packet_decode()` / `ads1299_packet_validate()` return:

- `ADS1299_PACKET_OK`
- `ADS1299_PACKET_ERR_ARGUMENT`
- `ADS1299_PACKET_ERR_SIZE`
- `ADS1299_PACKET_ERR_SYNC`
- `ADS1299_PACKET_ERR_VERSION`
- `ADS1299_PACKET_ERR_CRC`

A stream receiver should discard bytes until `A5 5A` is found, collect 49 bytes, validate the packet, and on failure resume sync search one byte after the previous candidate sync byte.

## Numerical conversion

`ads1299_code_to_volts(code, vref, gain)` is intended for quick engineering conversion and host visualization. Keep raw integer codes for storage and DSP whenever possible; convert to physical units at the processing boundary.

Example for a 4.5 V reference and gain 24:

```c
double ch1_uv = ads1299_code_to_microvolts(packet.channels[0], 4.5, 24);
```

The exact system-level scaling should match the board's reference topology and calibrated gain.
