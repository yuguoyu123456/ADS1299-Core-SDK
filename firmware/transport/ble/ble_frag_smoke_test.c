#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_ble_frag.h"
#include "ads1299_packet.h"

int main(void)
{
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint8_t rebuilt[ADS1299_PACKET_SIZE];
    uint8_t fragment[64];
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS] = {
        1, -2, 3, -4, 5, -6, 7, -8
    };
    ads1299_ble_reassembler_t rx;
    const size_t att_payload_max = 20u;
    size_t fragment_count;
    size_t i;

    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 ADS1299_PACKET_FLAG_DATA_VALID |
                                     ADS1299_PACKET_FLAG_TEST_SIGNAL,
                                 1234u,
                                 567890u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);

    fragment_count = ads1299_ble_fragment_count(sizeof(packet), att_payload_max);
    assert(fragment_count == 5u);

    ads1299_ble_reassembler_init(&rx);
    memset(rebuilt, 0, sizeof(rebuilt));

    for (i = 0u; i < fragment_count; ++i) {
        size_t n = ads1299_ble_make_fragment(packet,
                                             sizeof(packet),
                                             1234u,
                                             att_payload_max,
                                             (uint8_t)i,
                                             fragment,
                                             sizeof(fragment));
        int rc;
        assert(n > ADS1299_BLE_FRAG_HEADER_SIZE);
        rc = ads1299_ble_reassembler_push(&rx, fragment, n, rebuilt);
        assert(rc == (i + 1u == fragment_count ? 1 : 0));
    }

    assert(memcmp(packet, rebuilt, sizeof(packet)) == 0);
    assert(rx.completed_packets == 1u);
    assert(rx.dropped_packets == 0u);

    /* Missing/out-of-order fragment must be detected and counted. */
    ads1299_ble_reassembler_init(&rx);
    {
        size_t n0 = ads1299_ble_make_fragment(packet,
                                              sizeof(packet),
                                              1234u,
                                              att_payload_max,
                                              0u,
                                              fragment,
                                              sizeof(fragment));
        assert(n0 > 0u);
        assert(ads1299_ble_reassembler_push(&rx, fragment, n0, rebuilt) == 0);
    }
    {
        size_t n2 = ads1299_ble_make_fragment(packet,
                                              sizeof(packet),
                                              1234u,
                                              att_payload_max,
                                              2u,
                                              fragment,
                                              sizeof(fragment));
        assert(n2 > 0u);
        assert(ads1299_ble_reassembler_push(&rx, fragment, n2, rebuilt) < 0);
        assert(rx.dropped_packets == 1u);
    }

    /* A final CRC failure must reject the reconstructed packet. */
    ads1299_ble_reassembler_init(&rx);
    for (i = 0u; i < fragment_count; ++i) {
        size_t n = ads1299_ble_make_fragment(packet,
                                             sizeof(packet),
                                             4321u,
                                             att_payload_max,
                                             (uint8_t)i,
                                             fragment,
                                             sizeof(fragment));
        assert(n > 0u);
        if (i == fragment_count - 1u) {
            fragment[n - 1u] ^= 0x01u;
        }
        if (i + 1u == fragment_count) {
            assert(ads1299_ble_reassembler_push(&rx, fragment, n, rebuilt) == -9);
        } else {
            assert(ads1299_ble_reassembler_push(&rx, fragment, n, rebuilt) == 0);
        }
    }
    assert(rx.completed_packets == 0u);
    assert(rx.dropped_packets == 1u);

    return 0;
}
