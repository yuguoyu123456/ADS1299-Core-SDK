#include "ads1299_field_model.h"

#include <stddef.h>

#define FIELD(id_, name_, reg_, mask_, shift_, reset_, wr_, rel_, vmask_, valid_) \
    { (id_), (name_), (reg_), (mask_), (shift_), (reset_), \
      (uint8_t)((reg_) != ADS1299_REG_ID), (wr_), (rel_), (vmask_), (valid_) }

/*
 * Semantic fields from TI SBAS499C Rev. C. Reserved cells are intentionally not
 * represented as public fields: their required values belong to the register
 * safety model. valid_code_bitmap captures enumerated legal codes where useful.
 */
static const ads1299_field_info_t k_fields[ADS1299_FIELD_COUNT] = {
    FIELD(ADS1299_FIELD_ID_REV, "ID.REV", ADS1299_REG_ID, 0xE0u, 5u, 0u, 0u, 0u, 0u, 0u),
    FIELD(ADS1299_FIELD_ID_DEVICE, "ID.DEVICE", ADS1299_REG_ID, 0x0Cu, 2u, 0u, 0u, 0u, 0u, 0u),
    FIELD(ADS1299_FIELD_ID_CHANNEL_CODE, "ID.CHANNEL_CODE", ADS1299_REG_ID, 0x03u, 0u, 0u, 0u, 0u, 0u, 0x0007u),

    FIELD(ADS1299_FIELD_CONFIG1_DAISY_EN, "CONFIG1.DAISY_EN", ADS1299_REG_CONFIG1, 0x40u, 6u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG1_CLK_EN, "CONFIG1.CLK_EN", ADS1299_REG_CONFIG1, 0x20u, 5u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG1_DR, "CONFIG1.DR", ADS1299_REG_CONFIG1, 0x07u, 0u, ADS1299_DR_250SPS, 1u, 0u, 0u, 0x007Fu),

    FIELD(ADS1299_FIELD_CONFIG2_INT_CAL, "CONFIG2.INT_CAL", ADS1299_REG_CONFIG2, 0x10u, 4u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG2_CAL_AMP, "CONFIG2.CAL_AMP", ADS1299_REG_CONFIG2, 0x04u, 2u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG2_CAL_FREQ, "CONFIG2.CAL_FREQ", ADS1299_REG_CONFIG2, 0x03u, 0u, 0u, 1u, 0u, 0u, 0x000Bu),

    FIELD(ADS1299_FIELD_CONFIG3_PD_REFBUF, "CONFIG3.PD_REFBUF", ADS1299_REG_CONFIG3, 0x80u, 7u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG3_BIAS_MEAS, "CONFIG3.BIAS_MEAS", ADS1299_REG_CONFIG3, 0x10u, 4u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG3_BIASREF_INT, "CONFIG3.BIASREF_INT", ADS1299_REG_CONFIG3, 0x08u, 3u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG3_PD_BIAS, "CONFIG3.PD_BIAS", ADS1299_REG_CONFIG3, 0x04u, 2u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS, "CONFIG3.BIAS_LOFF_SENS", ADS1299_REG_CONFIG3, 0x02u, 1u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG3_BIAS_STAT, "CONFIG3.BIAS_STAT", ADS1299_REG_CONFIG3, 0x01u, 0u, 0u, 0u, 0u, 0u, 0x0003u),

    FIELD(ADS1299_FIELD_LOFF_COMP_TH, "LOFF.COMP_TH", ADS1299_REG_LOFF, 0xE0u, 5u, 0u, 1u, 0u, 0u, 0x00FFu),
    FIELD(ADS1299_FIELD_LOFF_CURRENT, "LOFF.ILEAD_OFF", ADS1299_REG_LOFF, 0x0Cu, 2u, 0u, 1u, 0u, 0u, 0x000Fu),
    FIELD(ADS1299_FIELD_LOFF_FREQ, "LOFF.FLEAD_OFF", ADS1299_REG_LOFF, 0x03u, 0u, 0u, 1u, 0u, 0u, 0x000Fu),

    FIELD(ADS1299_FIELD_CH_POWER_DOWN, "CHnSET.PDn", ADS1299_REG_CH1SET, 0x80u, 7u, 0u, 1u, 1u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CH_GAIN, "CHnSET.GAINn", ADS1299_REG_CH1SET, 0x70u, 4u, 6u, 1u, 1u, 0u, 0x007Fu),
    FIELD(ADS1299_FIELD_CH_SRB2, "CHnSET.SRB2", ADS1299_REG_CH1SET, 0x08u, 3u, 0u, 1u, 1u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CH_MUX, "CHnSET.MUXn", ADS1299_REG_CH1SET, 0x07u, 0u, 1u, 1u, 1u, 0u, 0x00FFu),

    FIELD(ADS1299_FIELD_BIAS_SENSP_MASK, "BIAS_SENSP.CH", ADS1299_REG_BIAS_SENSP, 0xFFu, 0u, 0u, 1u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_BIAS_SENSN_MASK, "BIAS_SENSN.CH", ADS1299_REG_BIAS_SENSN, 0xFFu, 0u, 0u, 1u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_LOFF_SENSP_MASK, "LOFF_SENSP.CH", ADS1299_REG_LOFF_SENSP, 0xFFu, 0u, 0u, 1u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_LOFF_SENSN_MASK, "LOFF_SENSN.CH", ADS1299_REG_LOFF_SENSN, 0xFFu, 0u, 0u, 1u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_LOFF_FLIP_MASK, "LOFF_FLIP.CH", ADS1299_REG_LOFF_FLIP, 0xFFu, 0u, 0u, 1u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_LOFF_STATP_MASK, "LOFF_STATP.CH", ADS1299_REG_LOFF_STATP, 0xFFu, 0u, 0u, 0u, 0u, 1u, 0u),
    FIELD(ADS1299_FIELD_LOFF_STATN_MASK, "LOFF_STATN.CH", ADS1299_REG_LOFF_STATN, 0xFFu, 0u, 0u, 0u, 0u, 1u, 0u),

    FIELD(ADS1299_FIELD_GPIO_DATA, "GPIO.GPIOD", ADS1299_REG_GPIO, 0xF0u, 4u, 0u, 1u, 0u, 0u, 0u),
    FIELD(ADS1299_FIELD_GPIO_DIRECTION, "GPIO.GPIOC", ADS1299_REG_GPIO, 0x0Fu, 0u, 0x0Fu, 1u, 0u, 0u, 0u),
    FIELD(ADS1299_FIELD_MISC1_SRB1, "MISC1.SRB1", ADS1299_REG_MISC1, 0x20u, 5u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG4_SINGLE_SHOT, "CONFIG4.SINGLE_SHOT", ADS1299_REG_CONFIG4, 0x08u, 3u, 0u, 1u, 0u, 0u, 0x0003u),
    FIELD(ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE, "CONFIG4.PD_LOFF_COMP", ADS1299_REG_CONFIG4, 0x02u, 1u, 0u, 1u, 0u, 0u, 0x0003u),
};

size_t ads1299_field_count(void) {
    return (size_t)ADS1299_FIELD_COUNT;
}

const ads1299_field_info_t *ads1299_field_info(ads1299_field_id_t field) {
    if ((unsigned)field >= (unsigned)ADS1299_FIELD_COUNT) return NULL;
    return &k_fields[(size_t)field];
}

int ads1299_field_register_address(ads1299_field_id_t field,
                                   uint8_t channel_1_to_8,
                                   ads1299_variant_t variant,
                                   uint8_t *address) {
    const ads1299_field_info_t *info = ads1299_field_info(field);
    if (!info || !address) return -1;
    if (!info->channel_relative) {
        *address = info->register_address;
        return 0;
    }

    const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
    if (channel_mask == 0u || channel_1_to_8 < 1u || channel_1_to_8 > 8u ||
        (channel_mask & ADS1299_CHANNEL_MASK(channel_1_to_8)) == 0u) {
        return -1;
    }
    *address = (uint8_t)(ADS1299_REG_CH1SET + channel_1_to_8 - 1u);
    return 0;
}

int ads1299_field_decode(ads1299_field_id_t field,
                         uint8_t register_value,
                         uint8_t *code) {
    const ads1299_field_info_t *info = ads1299_field_info(field);
    if (!info || !code) return -1;
    *code = (uint8_t)((register_value & info->mask) >> info->shift);
    return 0;
}

int ads1299_field_code_valid(ads1299_field_id_t field,
                             uint8_t code,
                             ads1299_variant_t variant) {
    const ads1299_field_info_t *info = ads1299_field_info(field);
    if (!info) return 0;
    const uint8_t max_code = (uint8_t)(info->mask >> info->shift);
    if (code > max_code) return 0;
    if (info->valid_code_bitmap != 0u &&
        (info->valid_code_bitmap & (uint16_t)(1u << code)) == 0u) {
        return 0;
    }
    if (info->variant_channel_masked) {
        const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
        if (channel_mask == 0u || (code & (uint8_t)~channel_mask) != 0u) return 0;
    }
    return 1;
}

int ads1299_field_encode(ads1299_field_id_t field,
                         uint8_t channel_1_to_8,
                         uint8_t code,
                         uint8_t current_register_value,
                         ads1299_variant_t variant,
                         uint8_t *encoded_register_value) {
    const ads1299_field_info_t *info = ads1299_field_info(field);
    uint8_t address = 0u;
    if (!info || !encoded_register_value || !info->writable ||
        ads1299_field_register_address(field, channel_1_to_8, variant, &address) != 0 ||
        !ads1299_field_code_valid(field, code, variant)) {
        return -1;
    }

    const uint8_t requested = (uint8_t)(
        (current_register_value & (uint8_t)~info->mask) |
        ((uint8_t)(code << info->shift) & info->mask));

    uint8_t sanitized = 0u;
    if (ads1299_sanitize_register_write(address, requested, variant, &sanitized) != 0)
        return -1;

    uint8_t round_trip_code = 0u;
    if (ads1299_field_decode(field, sanitized, &round_trip_code) != 0 ||
        round_trip_code != code) {
        return -1;
    }
    *encoded_register_value = sanitized;
    return 0;
}
