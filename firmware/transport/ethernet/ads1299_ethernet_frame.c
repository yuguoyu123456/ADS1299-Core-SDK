#include "ads1299_ethernet_frame.h"

#include <string.h>

static void put_u16le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static uint16_t get_u16le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static void put_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get_u32le(const uint8_t *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int ads1299_ethernet_frame_peek_size(const uint8_t *header,
                                     size_t header_length,
                                     size_t *frame_size_out)
{
    uint8_t packet_count;
    uint16_t payload_bytes;

    if (!header || !frame_size_out) return -1;
    if (header_length < ADS1299_ETH_FRAME_HEADER_SIZE) return -2;
    if (header[0] != ADS1299_ETH_FRAME_MAGIC0 ||
        header[1] != ADS1299_ETH_FRAME_MAGIC1 ||
        header[2] != ADS1299_ETH_FRAME_MAGIC2 ||
        header[3] != ADS1299_ETH_FRAME_MAGIC3) return -3;
    if (header[ADS1299_ETH_FRAME_OFFSET_VERSION] != ADS1299_ETH_FRAME_VERSION)
        return -4;

    packet_count = header[ADS1299_ETH_FRAME_OFFSET_PACKET_COUNT];
    payload_bytes = get_u16le(&header[ADS1299_ETH_FRAME_OFFSET_PAYLOAD_BYTES]);

    if (packet_count == 0u || packet_count > ADS1299_ETH_FRAME_MAX_PACKETS)
        return -5;
    if ((size_t)payload_bytes != (size_t)packet_count * ADS1299_PACKET_SIZE)
        return -6;

    *frame_size_out = ADS1299_ETH_FRAME_HEADER_SIZE + (size_t)payload_bytes;
    return 0;
}

size_t ads1299_ethernet_frame_encode(uint8_t *out,
                                     size_t out_capacity,
                                     uint32_t block_sequence,
                                     uint8_t flags,
                                     const uint8_t *packets,
                                     uint8_t packet_count)
{
    size_t payload_bytes;
    size_t frame_size;
    size_t i;

    if (!out || !packets) return 0u;
    if (packet_count == 0u || packet_count > ADS1299_ETH_FRAME_MAX_PACKETS)
        return 0u;

    payload_bytes = (size_t)packet_count * ADS1299_PACKET_SIZE;
    frame_size = ADS1299_ETH_FRAME_HEADER_SIZE + payload_bytes;
    if (out_capacity < frame_size) return 0u;

    for (i = 0u; i < packet_count; ++i) {
        const uint8_t *packet = packets + i * ADS1299_PACKET_SIZE;
        if (ads1299_packet_validate(packet, ADS1299_PACKET_SIZE) != ADS1299_PACKET_OK)
            return 0u;
    }

    memset(out, 0, ADS1299_ETH_FRAME_HEADER_SIZE);
    out[0] = ADS1299_ETH_FRAME_MAGIC0;
    out[1] = ADS1299_ETH_FRAME_MAGIC1;
    out[2] = ADS1299_ETH_FRAME_MAGIC2;
    out[3] = ADS1299_ETH_FRAME_MAGIC3;
    out[ADS1299_ETH_FRAME_OFFSET_VERSION] = ADS1299_ETH_FRAME_VERSION;
    out[ADS1299_ETH_FRAME_OFFSET_FLAGS] = flags;
    out[ADS1299_ETH_FRAME_OFFSET_PACKET_COUNT] = packet_count;
    put_u16le(&out[ADS1299_ETH_FRAME_OFFSET_PAYLOAD_BYTES], (uint16_t)payload_bytes);
    put_u32le(&out[ADS1299_ETH_FRAME_OFFSET_BLOCK_SEQUENCE], block_sequence);

    memcpy(&out[ADS1299_ETH_FRAME_HEADER_SIZE], packets, payload_bytes);
    return frame_size;
}

int ads1299_ethernet_frame_decode(const uint8_t *frame,
                                  size_t frame_length,
                                  ads1299_ethernet_frame_info_t *info,
                                  const uint8_t **packets)
{
    size_t expected_size;
    uint8_t packet_count;
    size_t i;
    int rc;

    if (!frame || !info || !packets) return -1;
    rc = ads1299_ethernet_frame_peek_size(frame, frame_length, &expected_size);
    if (rc != 0) return rc;
    if (frame_length != expected_size) return -7;

    packet_count = frame[ADS1299_ETH_FRAME_OFFSET_PACKET_COUNT];
    for (i = 0u; i < packet_count; ++i) {
        const uint8_t *packet = &frame[ADS1299_ETH_FRAME_HEADER_SIZE +
                                      i * ADS1299_PACKET_SIZE];
        if (ads1299_packet_validate(packet, ADS1299_PACKET_SIZE) != ADS1299_PACKET_OK)
            return -8;
    }

    info->block_sequence = get_u32le(&frame[ADS1299_ETH_FRAME_OFFSET_BLOCK_SEQUENCE]);
    info->flags = frame[ADS1299_ETH_FRAME_OFFSET_FLAGS];
    info->packet_count = packet_count;
    info->payload_bytes = get_u16le(&frame[ADS1299_ETH_FRAME_OFFSET_PAYLOAD_BYTES]);
    *packets = &frame[ADS1299_ETH_FRAME_HEADER_SIZE];
    return 0;
}
