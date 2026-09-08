#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_ethernet_frame.h"
#include "ads1299_packet.h"

static void make_packet(uint8_t out[ADS1299_PACKET_SIZE], uint32_t sequence, int32_t base)
{
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS];
    size_t i;

    for (i = 0u; i < ADS1299_PACKET_CHANNELS; ++i)
        channels[i] = base + (int32_t)i;

    assert(ads1299_packet_encode(out,
                                 ADS1299_PACKET_SIZE,
                                 ADS1299_PACKET_FLAG_DATA_VALID |
                                     ADS1299_PACKET_FLAG_TIMESTAMP_VALID,
                                 sequence,
                                 sequence * 4000u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);
}

int main(void)
{
    uint8_t packets[8u * ADS1299_PACKET_SIZE];
    uint8_t frame[ADS1299_ETH_FRAME_MAX_SIZE];
    ads1299_ethernet_frame_info_t info;
    const uint8_t *decoded_packets = 0;
    size_t encoded;
    size_t expected_size = 0u;
    size_t i;

    for (i = 0u; i < 8u; ++i)
        make_packet(&packets[i * ADS1299_PACKET_SIZE], (uint32_t)(100u + i), (int32_t)(i * 100));

    encoded = ads1299_ethernet_frame_encode(frame,
                                             sizeof(frame),
                                             77u,
                                             0x01u,
                                             packets,
                                             8u);
    assert(encoded == ADS1299_ETH_FRAME_HEADER_SIZE + 8u * ADS1299_PACKET_SIZE);

    assert(ads1299_ethernet_frame_peek_size(frame,
                                            ADS1299_ETH_FRAME_HEADER_SIZE,
                                            &expected_size) == 0);
    assert(expected_size == encoded);

    assert(ads1299_ethernet_frame_decode(frame,
                                         encoded,
                                         &info,
                                         &decoded_packets) == 0);
    assert(info.block_sequence == 77u);
    assert(info.flags == 0x01u);
    assert(info.packet_count == 8u);
    assert(info.payload_bytes == 8u * ADS1299_PACKET_SIZE);
    assert(memcmp(decoded_packets, packets, sizeof(packets)) == 0);

    /* Corrupt one canonical packet and confirm the Ethernet frame is rejected. */
    frame[ADS1299_ETH_FRAME_HEADER_SIZE + ADS1299_PACKET_SIZE + 20u] ^= 0x01u;
    assert(ads1299_ethernet_frame_decode(frame,
                                         encoded,
                                         &info,
                                         &decoded_packets) == -8);

    return 0;
}
