#include "ads1299_packet.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message) {
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

int main(void) {
    static const uint8_t expected_packet[ADS1299_PACKET_SIZE] = {
        0xA5, 0x5A, 0x00, 0x03,
        0x04, 0x03, 0x02, 0x01,
        0x0D, 0x0C, 0x0B, 0x0A,
        0xC0, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x56, 0x34, 0x12, 0x00,
        0xAA, 0xCB, 0xED, 0xFF,
        0xFF, 0xFF, 0x7F, 0x00,
        0x00, 0x00, 0x80, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0x2A, 0x00, 0x00, 0x00,
        0x44, 0x29
    };
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0, 0x00, 0x00};
    const int32_t channels[ADS1299_PACKET_CHANNELS] = {
        1, -1, 0x00123456, -0x00123456,
        8388607, -8388608, 0, 42
    };
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint8_t corrupted[ADS1299_PACKET_SIZE];
    ads1299_packet_decoded_t decoded;
    size_t encoded_len;
    size_t i;

    encoded_len = ads1299_packet_encode(packet,
                                        sizeof(packet),
                                        0x03,
                                        0x01020304u,
                                        0x0A0B0C0Du,
                                        status,
                                        channels);
    if (encoded_len != ADS1299_PACKET_SIZE) return fail("encoded length is not 49 bytes");
    if (memcmp(packet, expected_packet, ADS1299_PACKET_SIZE) != 0) {
        return fail("encoded bytes differ from canonical test vector");
    }
    if (!ads1299_packet_validate(packet, sizeof(packet))) return fail("canonical packet did not validate");
    if (!ads1299_packet_decode(packet, sizeof(packet), &decoded)) return fail("canonical packet did not decode");

    if (decoded.version != ADS1299_PACKET_VERSION) return fail("version mismatch");
    if (decoded.flags != 0x03u) return fail("flags mismatch");
    if (decoded.sequence != 0x01020304u) return fail("sequence little-endian decode mismatch");
    if (decoded.timestamp_us != 0x0A0B0C0Du) return fail("timestamp little-endian decode mismatch");
    if (memcmp(decoded.status, status, sizeof(status)) != 0) return fail("status bytes mismatch");
    for (i = 0; i < ADS1299_PACKET_CHANNELS; ++i) {
        if (decoded.channels[i] != channels[i]) return fail("signed channel decode mismatch");
    }

    memcpy(corrupted, packet, sizeof(corrupted));
    corrupted[ADS1299_PACKET_OFFSET_CHANNELS + 5u] ^= 0x01u;
    if (ads1299_packet_validate(corrupted, sizeof(corrupted))) return fail("CRC did not reject corrupted payload");

    memcpy(corrupted, packet, sizeof(corrupted));
    corrupted[ADS1299_PACKET_OFFSET_SYNC0] = 0x00u;
    if (ads1299_packet_validate(corrupted, sizeof(corrupted))) return fail("sync check accepted invalid packet");

    memcpy(corrupted, packet, sizeof(corrupted));
    corrupted[ADS1299_PACKET_OFFSET_VERSION] = (uint8_t)(ADS1299_PACKET_VERSION + 1u);
    if (ads1299_packet_validate(corrupted, sizeof(corrupted))) return fail("version check accepted unsupported packet");

    if (ads1299_packet_encode(NULL, 0u, 0u, 0u, 0u, status, channels) != 0u) {
        return fail("NULL encode guard failed");
    }
    if (ads1299_packet_validate(packet, ADS1299_PACKET_SIZE - 1u)) {
        return fail("short packet length guard failed");
    }

    printf("PASS: ADS1299 canonical 49-byte packet encode/validate/decode self-test\n");
    printf("PASS: CRC16-CCITT vector = 0x2944 (wire bytes 44 29)\n");
    return 0;
}
