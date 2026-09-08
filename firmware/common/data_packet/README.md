# ADS1299 canonical data packet

This leaf module is the single C reference implementation of the repository's canonical one-device transport packet. MCU, FPGA bridge, Linux, USB, UART, BLE, Wi-Fi and host tooling should use this byte contract instead of defining local variants.

## Wire contract

One ADS1299-8 packet is exactly **49 bytes**:

| Offset | Size | Field | Encoding |
|---:|---:|---|---|
| 0 | 1 | sync0 | `0xA5` |
| 1 | 1 | sync1 | `0x5A` |
| 2 | 1 | version | currently `0x00` |
| 3 | 1 | flags | application flags |
| 4 | 4 | sequence | unsigned 32-bit, little-endian |
| 8 | 4 | timestamp_us | unsigned 32-bit, little-endian |
| 12 | 3 | ADS1299 status | raw 3 status bytes from the 27-byte ADS1299 frame |
| 15 | 32 | channels | eight signed 32-bit samples, little-endian |
| 47 | 2 | CRC16 | CRC16-CCITT, little-endian on wire |

The CRC starts at byte 2 (`version`) and ends at byte 46 (last channel byte). The two sync bytes and the CRC field itself are not included in the CRC calculation. CRC parameters are polynomial `0x1021`, initial value `0xFFFF`, no reflected input/output, no final XOR.

ADS1299 raw channel samples are 24-bit two's-complement values. Sign-extend each raw channel to `int32_t` before calling `ads1299_packet_encode()`; do not store the three raw ADC bytes directly into the 32-bit packet field.

## Files to use

For an embedded target, copy or compile:

- `ads1299_packet.h`
- `ads1299_packet.c`

Call `ads1299_packet_encode()` after one complete 27-byte ADS1299-8 frame has been captured and decoded. Use `ads1299_packet_validate()` and `ads1299_packet_decode()` on receivers or loopback diagnostics.

The implementation is freestanding except for standard `<stddef.h>` and `<stdint.h>` integer definitions. It performs no allocation, logging, transport I/O or floating-point work.

## Minimal sender path

```c
uint8_t tx[ADS1299_PACKET_SIZE];
uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {status0, status1, status2};
int32_t channels[ADS1299_PACKET_CHANNELS] = {ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8};

size_t n = ads1299_packet_encode(tx, sizeof(tx), flags,
                                 sequence++, timestamp_us,
                                 status, channels);
if (n == ADS1299_PACKET_SIZE) {
    transport_write(tx, n); /* Replace only with the platform transport call. */
}
```

`transport_write()` above is intentionally not part of this common leaf. UART/USB/BLE/Wi-Fi implementations belong to their platform/transport folders so this packet contract remains portable.

## Minimal receiver path

```c
ads1299_packet_decoded_t frame;
if (ads1299_packet_decode(rx, rx_len, &frame)) {
    /* frame.channels[0..7], frame.status, sequence and timestamp are valid. */
} else {
    /* Wrong length, sync, version, or CRC. Drop/resynchronize the packet. */
}
```

## Host smoke test (no ADS1299 hardware required)

From this directory on Linux/macOS, or a Windows shell with GCC/Clang/MinGW available:

```sh
make -f Makefile.host test
```

Equivalent direct command:

```sh
cc -std=c99 -Wall -Wextra -Werror -pedantic -O2 -I. \
  ads1299_packet.c ads1299_packet_selftest.c -o ads1299_packet_selftest
./ads1299_packet_selftest
```

Expected output:

```text
PASS: ADS1299 canonical 49-byte packet encode/validate/decode self-test
PASS: CRC16-CCITT vector = 0x2944 (wire bytes 44 29)
```

The test uses a fixed packet containing positive, negative, maximum 24-bit and minimum 24-bit channel values. It verifies the exact 49-byte output, little-endian sequence/timestamp/channels, signed channel round-trip, CRC corruption rejection, sync rejection, version rejection and basic invalid-input guards.

Canonical fixed vector used by the test:

```text
a5 5a 00 03 04 03 02 01 0d 0c 0b 0a c0 00 00
01 00 00 00 ff ff ff ff 56 34 12 00 aa cb ed ff
ff ff 7f 00 00 00 80 ff 00 00 00 00 2a 00 00 00 44 29
```

## ESP-IDF integration

The existing `CMakeLists.txt` registers `ads1299_packet.c` as an ESP-IDF component. Add this directory to the component search path according to the owning ESP-IDF project and include:

```c
#include "ads1299_packet.h"
```

Other build systems can compile `ads1299_packet.c` directly; there is no ESP-IDF dependency inside the C implementation.

## Failure diagnosis

- **Length is not 49:** the transport framed the stream incorrectly or a producer emitted a non-canonical packet.
- **Sync mismatch:** scan for the ordered bytes `A5 5A`, then collect exactly 49 bytes before validation.
- **Version mismatch:** do not silently parse a future protocol version as version 0.
- **CRC mismatch:** suspect byte loss/insertion, transport corruption, wrong CRC coverage, wrong endian order for the two CRC bytes, or a local packet variant.
- **Channels have impossible large values:** check 24-bit ADS1299 sign extension before packet encoding and confirm little-endian 32-bit transport decoding.
- **Sequence gaps:** treat this as a producer/queue/transport overrun diagnostic; do not hide it by rewriting sequence numbers downstream.
- **Timestamp jumps/wrap:** `timestamp_us` is a 32-bit transport field and naturally wraps. Consumers should handle unsigned wraparound.

## Validation boundary

Passing `ads1299_packet_selftest` validates the software packet contract only. It does **not** prove ADS1299 electrical bring-up, SPI timing, DRDY handling, sampling accuracy, board signal integrity, EMC/safety compliance, or medical-device suitability. Those checks belong to the owning hardware/platform validation flow.
