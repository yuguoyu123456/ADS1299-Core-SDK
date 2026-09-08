#include "ads1299_packet.h"

static void put_u32_le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

static uint32_t get_u32_le(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

uint16_t ads1299_crc16_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFFu;
    if (!data && len) return 0u;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ 0x1021u)
                                  : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

size_t ads1299_packet_encode(uint8_t *out,
                             size_t capacity,
                             uint8_t flags,
                             uint32_t sequence,
                             uint32_t timestamp_us,
                             const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                             const int32_t channels[ADS1299_PACKET_CHANNELS]) {
    if (!out || !status || !channels || capacity < ADS1299_PACKET_SIZE) return 0u;

    size_t i = 0;
    out[i++] = ADS1299_PACKET_SYNC0;
    out[i++] = ADS1299_PACKET_SYNC1;
    out[i++] = ADS1299_PACKET_VERSION;
    out[i++] = flags;
    put_u32_le(&out[i], sequence); i += 4;
    put_u32_le(&out[i], timestamp_us); i += 4;
    for (size_t k = 0; k < ADS1299_PACKET_STATUS_BYTES; ++k) out[i++] = status[k];
    for (size_t ch = 0; ch < ADS1299_PACKET_CHANNELS; ++ch) {
        put_u32_le(&out[i], (uint32_t)channels[ch]);
        i += 4;
    }
    const uint16_t crc = ads1299_crc16_ccitt(&out[2], i - 2u);
    out[i++] = (uint8_t)(crc & 0xFFu);
    out[i++] = (uint8_t)(crc >> 8);
    return i;
}

int ads1299_packet_validate(const uint8_t *packet, size_t len) {
    uint16_t expected_crc;
    uint16_t packet_crc;

    if (!packet || len != ADS1299_PACKET_SIZE) return 0;
    if (packet[ADS1299_PACKET_OFFSET_SYNC0] != ADS1299_PACKET_SYNC0 ||
        packet[ADS1299_PACKET_OFFSET_SYNC1] != ADS1299_PACKET_SYNC1) return 0;
    if (packet[ADS1299_PACKET_OFFSET_VERSION] != ADS1299_PACKET_VERSION) return 0;

    expected_crc = ads1299_crc16_ccitt(&packet[ADS1299_PACKET_OFFSET_VERSION],
                                       ADS1299_PACKET_OFFSET_CRC - ADS1299_PACKET_OFFSET_VERSION);
    packet_crc = (uint16_t)packet[ADS1299_PACKET_OFFSET_CRC] |
                 ((uint16_t)packet[ADS1299_PACKET_OFFSET_CRC + 1u] << 8);
    return expected_crc == packet_crc;
}

int ads1299_packet_decode(const uint8_t *packet,
                          size_t len,
                          ads1299_packet_decoded_t *decoded) {
    size_t ch;

    if (!decoded || !ads1299_packet_validate(packet, len)) return 0;

    decoded->version = packet[ADS1299_PACKET_OFFSET_VERSION];
    decoded->flags = packet[ADS1299_PACKET_OFFSET_FLAGS];
    decoded->sequence = get_u32_le(&packet[ADS1299_PACKET_OFFSET_SEQUENCE]);
    decoded->timestamp_us = get_u32_le(&packet[ADS1299_PACKET_OFFSET_TIMESTAMP]);

    for (ch = 0; ch < ADS1299_PACKET_STATUS_BYTES; ++ch) {
        decoded->status[ch] = packet[ADS1299_PACKET_OFFSET_STATUS + ch];
    }
    for (ch = 0; ch < ADS1299_PACKET_CHANNELS; ++ch) {
        decoded->channels[ch] = (int32_t)get_u32_le(
            &packet[ADS1299_PACKET_OFFSET_CHANNELS + (ch * 4u)]);
    }
    return 1;
}
