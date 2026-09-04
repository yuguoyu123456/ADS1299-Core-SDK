#include "ads1299_frame.h"

#include <string.h>

static uint8_t variant_channel_count(ads1299_variant_t variant) {
    switch (variant) {
        case ADS1299_VARIANT_4CH: return 4u;
        case ADS1299_VARIANT_6CH: return 6u;
        case ADS1299_VARIANT_8CH: return 8u;
        default: return 0u;
    }
}

size_t ads1299_frame_bytes_for_variant(ads1299_variant_t variant) {
    const uint8_t channels = variant_channel_count(variant);
    if (channels == 0u) return 0u;
    return ADS1299_STATUS_BYTES + (size_t)channels * ADS1299_BYTES_PER_CHANNEL;
}

ads1299_status_t ads1299_decode_frame_status(const uint8_t raw_status[ADS1299_STATUS_BYTES],
                                             ads1299_variant_t variant,
                                             ads1299_frame_status_t *status) {
    if (!raw_status || !status) return ADS1299_EINVAL;
    const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
    if (channel_mask == 0u) return ADS1299_EINVAL;

    status->header_nibble = (uint8_t)(raw_status[0] >> 4);
    status->loff_statp = (uint8_t)(((raw_status[0] & 0x0Fu) << 4) |
                                   ((raw_status[1] >> 4) & 0x0Fu));
    status->loff_statn = (uint8_t)(((raw_status[1] & 0x0Fu) << 4) |
                                   ((raw_status[2] >> 4) & 0x0Fu));
    status->gpio_data = (uint8_t)(raw_status[2] & 0x0Fu);
    status->loff_statp &= channel_mask;
    status->loff_statn &= channel_mask;
    status->header_valid = (uint8_t)(status->header_nibble == 0x0Cu);
    return status->header_valid ? ADS1299_OK : ADS1299_EVERIFY;
}

ads1299_status_t ads1299_decode_frame_variant(const uint8_t *raw,
                                              size_t raw_len,
                                              ads1299_variant_t variant,
                                              ads1299_frame_t *frame,
                                              ads1299_frame_status_t *status) {
    if (!raw || !frame) return ADS1299_EINVAL;
    const uint8_t channels = variant_channel_count(variant);
    const size_t expected = ads1299_frame_bytes_for_variant(variant);
    if (channels == 0u || raw_len != expected) return ADS1299_EINVAL;

    memset(frame, 0, sizeof(*frame));
    memcpy(frame->status, raw, ADS1299_STATUS_BYTES);

    ads1299_frame_status_t local_status;
    ads1299_status_t src = ads1299_decode_frame_status(raw, variant,
                                                       status ? status : &local_status);
    if (src != ADS1299_OK) return src;

    for (uint8_t ch = 0u; ch < channels; ++ch) {
        const size_t i = ADS1299_STATUS_BYTES + (size_t)ch * ADS1299_BYTES_PER_CHANNEL;
        const uint32_t value24 = ((uint32_t)raw[i] << 16) |
                                 ((uint32_t)raw[i + 1u] << 8) |
                                 (uint32_t)raw[i + 2u];
        frame->channel[ch] = ads1299_sign_extend24(value24);
    }
    return ADS1299_OK;
}
