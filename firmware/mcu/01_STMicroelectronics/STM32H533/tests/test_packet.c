#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "ads1299_packet.h"

void test_canonical_packet_round_trip(void) {
    uint8_t packet[ADS1299_PACKET_SIZE];
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    const int32_t channels[ADS1299_PACKET_CHANNELS] = {
        0, 1, -1, 0x007FFFFF, -0x00800000, 123456, -654321, 42
    };
    ads1299_packet_decoded_t decoded;
    size_t encoded;

    encoded = ads1299_packet_encode(packet,
                                    sizeof(packet),
                                    0x03u,
                                    0x12345678u,
                                    0x89ABCDEFu,
                                    status,
                                    channels);
    assert(encoded == ADS1299_PACKET_SIZE);
    assert(packet[ADS1299_PACKET_OFFSET_SYNC0] == ADS1299_PACKET_SYNC0);
    assert(packet[ADS1299_PACKET_OFFSET_SYNC1] == ADS1299_PACKET_SYNC1);
    assert(ads1299_packet_validate(packet, sizeof(packet)) == 1);
    assert(ads1299_packet_decode(packet, sizeof(packet), &decoded) == 1);

    assert(decoded.version == ADS1299_PACKET_VERSION);
    assert(decoded.flags == 0x03u);
    assert(decoded.sequence == 0x12345678u);
    assert(decoded.timestamp_us == 0x89ABCDEFu);
    assert(memcmp(decoded.status, status, sizeof(status)) == 0);
    assert(memcmp(decoded.channels, channels, sizeof(channels)) == 0);

    packet[ADS1299_PACKET_OFFSET_CHANNELS] ^= 0x01u;
    assert(ads1299_packet_validate(packet, sizeof(packet)) == 0);
}
