#include "ads1299_packet.h"

#include <string.h>

static void put_u16_le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
}

static uint16_t get_u16_le(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

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

    size_t i = 0u;
    out[i++] = ADS1299_PACKET_SYNC0;
    out[i++] = ADS1299_PACKET_SYNC1;
    out[i++] = ADS1299_PACKET_VERSION;
    out[i++] = flags;
    put_u32_le(&out[i], sequence);
    i += 4u;
    put_u32_le(&out[i], timestamp_us);
    i += 4u;

    for (size_t k = 0; k < ADS1299_PACKET_STATUS_BYTES; ++k) {
        out[i++] = status[k];
    }

    for (size_t ch = 0; ch < ADS1299_PACKET_CHANNELS; ++ch) {
        put_u32_le(&out[i], (uint32_t)channels[ch]);
        i += 4u;
    }

    const uint16_t crc = ads1299_crc16_ccitt(&out[ADS1299_PACKET_OFFSET_VERSION],
                                             ADS1299_PACKET_OFFSET_CRC - ADS1299_PACKET_OFFSET_VERSION);
    put_u16_le(&out[i], crc);
    i += 2u;
    return i;
}

size_t ads1299_packet_encode_struct(uint8_t *out,
                                    size_t capacity,
                                    const ads1299_packet_t *packet) {
    if (!packet) return 0u;
    return ads1299_packet_encode(out,
                                 capacity,
                                 packet->flags,
                                 packet->sequence,
                                 packet->timestamp_us,
                                 packet->status,
                                 packet->channels);
}

ads1299_packet_result_t ads1299_packet_validate(const uint8_t *data, size_t len) {
    if (!data) return ADS1299_PACKET_ERR_ARGUMENT;
    if (len < ADS1299_PACKET_SIZE) return ADS1299_PACKET_ERR_SIZE;
    if (data[ADS1299_PACKET_OFFSET_SYNC0] != ADS1299_PACKET_SYNC0 ||
        data[ADS1299_PACKET_OFFSET_SYNC1] != ADS1299_PACKET_SYNC1) {
        return ADS1299_PACKET_ERR_SYNC;
    }
    if (data[ADS1299_PACKET_OFFSET_VERSION] != ADS1299_PACKET_VERSION) {
        return ADS1299_PACKET_ERR_VERSION;
    }

    const uint16_t expected = ads1299_crc16_ccitt(
        &data[ADS1299_PACKET_OFFSET_VERSION],
        ADS1299_PACKET_OFFSET_CRC - ADS1299_PACKET_OFFSET_VERSION);
    const uint16_t actual = get_u16_le(&data[ADS1299_PACKET_OFFSET_CRC]);
    if (actual != expected) return ADS1299_PACKET_ERR_CRC;
    return ADS1299_PACKET_OK;
}

ads1299_packet_result_t ads1299_packet_decode(const uint8_t *data,
                                               size_t len,
                                               ads1299_packet_t *out) {
    if (!out) return ADS1299_PACKET_ERR_ARGUMENT;

    const ads1299_packet_result_t valid = ads1299_packet_validate(data, len);
    if (valid != ADS1299_PACKET_OK) return valid;

    memset(out, 0, sizeof(*out));
    out->version = data[ADS1299_PACKET_OFFSET_VERSION];
    out->flags = data[ADS1299_PACKET_OFFSET_FLAGS];
    out->sequence = get_u32_le(&data[ADS1299_PACKET_OFFSET_SEQUENCE]);
    out->timestamp_us = get_u32_le(&data[ADS1299_PACKET_OFFSET_TIMESTAMP]);
    memcpy(out->status,
           &data[ADS1299_PACKET_OFFSET_STATUS],
           ADS1299_PACKET_STATUS_BYTES);

    for (size_t ch = 0; ch < ADS1299_PACKET_CHANNELS; ++ch) {
        const size_t offset = ADS1299_PACKET_OFFSET_CHANNELS + ch * 4u;
        out->channels[ch] = (int32_t)get_u32_le(&data[offset]);
    }

    out->crc = get_u16_le(&data[ADS1299_PACKET_OFFSET_CRC]);
    return ADS1299_PACKET_OK;
}

int32_t ads1299_sign_extend24(uint32_t raw24) {
    raw24 &= 0x00FFFFFFu;
    if (raw24 & 0x00800000u) raw24 |= 0xFF000000u;
    return (int32_t)raw24;
}

int32_t ads1299_decode_sample24_be(const uint8_t raw[3]) {
    if (!raw) return 0;
    const uint32_t value = ((uint32_t)raw[0] << 16) |
                           ((uint32_t)raw[1] << 8) |
                           (uint32_t)raw[2];
    return ads1299_sign_extend24(value);
}

void ads1299_encode_sample24_be(int32_t sample, uint8_t raw[3]) {
    if (!raw) return;
    const uint32_t value = (uint32_t)sample & 0x00FFFFFFu;
    raw[0] = (uint8_t)(value >> 16);
    raw[1] = (uint8_t)(value >> 8);
    raw[2] = (uint8_t)value;
}

bool ads1299_decode_raw_frame(const uint8_t *raw,
                              size_t raw_len,
                              uint8_t status[ADS1299_PACKET_STATUS_BYTES],
                              int32_t channels[ADS1299_PACKET_CHANNELS]) {
    const size_t required = ADS1299_PACKET_STATUS_BYTES + ADS1299_PACKET_CHANNELS * 3u;
    if (!raw || !status || !channels || raw_len < required) return false;

    memcpy(status, raw, ADS1299_PACKET_STATUS_BYTES);
    for (size_t ch = 0; ch < ADS1299_PACKET_CHANNELS; ++ch) {
        channels[ch] = ads1299_decode_sample24_be(&raw[ADS1299_PACKET_STATUS_BYTES + ch * 3u]);
    }
    return true;
}

double ads1299_code_to_volts(int32_t code, double vref_volts, unsigned gain) {
    if (gain == 0u || vref_volts <= 0.0) return 0.0;
    return ((double)code * vref_volts) / ((double)gain * 8388607.0);
}

double ads1299_code_to_microvolts(int32_t code, double vref_volts, unsigned gain) {
    return ads1299_code_to_volts(code, vref_volts, gain) * 1000000.0;
}
