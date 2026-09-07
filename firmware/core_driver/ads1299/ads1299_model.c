/* Consolidated ADS1299 register/field/semantic model. */

/* ===== register model ===== */
#include "ads1299_model.h"

#include <stddef.h>

#define REG_INFO(addr, reset, known, writable, ones, zeros, ro) \
    { (addr), (reset), (known), (writable), (ones), (zeros), (ro) }

static const ads1299_register_info_t k_registers[ADS1299_REGISTER_COUNT] = {
    REG_INFO(ADS1299_REG_ID,         0x00u,                    0u, ADS1299_ID_WRITABLE_MASK,           0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_CONFIG1,    ADS1299_RESET_CONFIG1,    1u, ADS1299_CONFIG1_WRITABLE_MASK,      0x90u, 0x08u, 0u),
    REG_INFO(ADS1299_REG_CONFIG2,    ADS1299_RESET_CONFIG2,    1u, ADS1299_CONFIG2_WRITABLE_MASK,      0xC0u, 0x28u, 0u),
    REG_INFO(ADS1299_REG_CONFIG3,    ADS1299_RESET_CONFIG3,    1u, ADS1299_CONFIG3_WRITABLE_MASK,      0x60u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF,       ADS1299_RESET_LOFF,       1u, ADS1299_LOFF_WRITABLE_MASK,         0x00u, 0x10u, 0u),
    REG_INFO(ADS1299_REG_CH1SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH2SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH3SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH4SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH5SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH6SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH7SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_CH8SET,     ADS1299_RESET_CHNSET,     1u, ADS1299_CHNSET_WRITABLE_MASK,       0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_BIAS_SENSP, ADS1299_RESET_BIAS_SENSP, 1u, ADS1299_CHANNEL_BITS_WRITABLE_MASK, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_BIAS_SENSN, ADS1299_RESET_BIAS_SENSN, 1u, ADS1299_CHANNEL_BITS_WRITABLE_MASK, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_SENSP, ADS1299_RESET_LOFF_SENSP, 1u, ADS1299_CHANNEL_BITS_WRITABLE_MASK, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_SENSN, ADS1299_RESET_LOFF_SENSN, 1u, ADS1299_CHANNEL_BITS_WRITABLE_MASK, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_FLIP,  ADS1299_RESET_LOFF_FLIP,  1u, ADS1299_CHANNEL_BITS_WRITABLE_MASK, 0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_LOFF_STATP, ADS1299_RESET_LOFF_STATP, 1u, ADS1299_LOFF_STAT_WRITABLE_MASK,    0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_LOFF_STATN, ADS1299_RESET_LOFF_STATN, 1u, ADS1299_LOFF_STAT_WRITABLE_MASK,    0x00u, 0x00u, 1u),
    REG_INFO(ADS1299_REG_GPIO,       ADS1299_RESET_GPIO,       1u, ADS1299_GPIO_WRITABLE_MASK,         0x00u, 0x00u, 0u),
    REG_INFO(ADS1299_REG_MISC1,      ADS1299_RESET_MISC1,      1u, ADS1299_MISC1_WRITABLE_MASK,        0x00u, 0xDFu, 0u),
    REG_INFO(ADS1299_REG_MISC2,      ADS1299_RESET_MISC2,      1u, ADS1299_MISC2_WRITABLE_MASK,        0x00u, 0xFFu, 0u),
    REG_INFO(ADS1299_REG_CONFIG4,    ADS1299_RESET_CONFIG4,    1u, ADS1299_CONFIG4_WRITABLE_MASK,      0x00u, 0xF5u, 0u),
};

const ads1299_register_info_t *ads1299_register_info(uint8_t address) {
    if (address > ADS1299_REG_LAST) return NULL;
    return &k_registers[address];
}

uint8_t ads1299_variant_channel_mask(ads1299_variant_t variant) {
    switch (variant) {
        case ADS1299_VARIANT_4CH: return 0x0Fu;
        case ADS1299_VARIANT_6CH: return 0x3Fu;
        case ADS1299_VARIANT_8CH: return 0xFFu;
        default: return 0x00u;
    }
}

int ads1299_register_available(uint8_t address, ads1299_variant_t variant) {
    if (address > ADS1299_REG_LAST || ads1299_variant_channel_mask(variant) == 0u) {
        return 0;
    }
    if (address == ADS1299_REG_CH5SET || address == ADS1299_REG_CH6SET) {
        return variant == ADS1299_VARIANT_6CH || variant == ADS1299_VARIANT_8CH;
    }
    if (address == ADS1299_REG_CH7SET || address == ADS1299_REG_CH8SET) {
        return variant == ADS1299_VARIANT_8CH;
    }
    return 1;
}

static int is_channel_mask_register(uint8_t address) {
    return address == ADS1299_REG_BIAS_SENSP || address == ADS1299_REG_BIAS_SENSN ||
           address == ADS1299_REG_LOFF_SENSP || address == ADS1299_REG_LOFF_SENSN ||
           address == ADS1299_REG_LOFF_FLIP;
}

static int is_channel_settings_register(uint8_t address) {
    return address >= ADS1299_REG_CH1SET && address <= ADS1299_REG_CH8SET;
}

static int semantic_fields_valid(uint8_t address, uint8_t value) {
    if (address == ADS1299_REG_CONFIG1) {
        return (value & ADS1299_CONFIG1_DR_MASK) != ADS1299_DR_RESERVED;
    }
    if (address == ADS1299_REG_CONFIG2) {
        return (value & ADS1299_CONFIG2_CAL_FREQ_MASK) != ADS1299_TEST_FREQ_RESERVED;
    }
    if (is_channel_settings_register(address)) {
        return (value & ADS1299_CH_GAIN_MASK) != ADS1299_GAIN_DO_NOT_USE;
    }
    return 1;
}

int ads1299_register_write_value_valid(uint8_t address,
                                       uint8_t value,
                                       ads1299_variant_t variant) {
    const ads1299_register_info_t *info = ads1299_register_info(address);
    const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
    if (!info || channel_mask == 0u || info->read_only ||
        !ads1299_register_available(address, variant)) {
        return 0;
    }

    if ((value & info->required_one_mask) != info->required_one_mask) return 0;
    if ((value & info->required_zero_mask) != 0u) return 0;
    if ((value & (uint8_t)~(info->writable_mask | info->required_one_mask)) != 0u) return 0;
    if (is_channel_mask_register(address) && (value & (uint8_t)~channel_mask) != 0u) return 0;
    return semantic_fields_valid(address, value);
}

int ads1299_sanitize_register_write(uint8_t address,
                                    uint8_t requested,
                                    ads1299_variant_t variant,
                                    uint8_t *sanitized) {
    const ads1299_register_info_t *info = ads1299_register_info(address);
    const uint8_t channel_mask = ads1299_variant_channel_mask(variant);
    if (!info || !sanitized || channel_mask == 0u || info->read_only ||
        !ads1299_register_available(address, variant)) {
        return -1;
    }

    uint8_t value = (uint8_t)(requested & info->writable_mask);
    value = (uint8_t)(value | info->required_one_mask);
    value = (uint8_t)(value & (uint8_t)~info->required_zero_mask);
    if (is_channel_mask_register(address)) value &= channel_mask;
    if (!semantic_fields_valid(address, value)) return -1;
    *sanitized = value;
    return 0;
}

/* ===== field model ===== */
#include "ads1299_model.h"

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

/* ===== semantics ===== */
#include "ads1299_model.h"

#include <stddef.h>

static int context_has(const ads1299_semantic_context_t *context, uint8_t mask) {
    return context && (context->valid_mask & mask) == mask;
}

static void descriptor_reset(ads1299_code_descriptor_t *descriptor,
                             ads1299_field_id_t field) {
    descriptor->name = NULL;
    descriptor->meaning = ads1299_field_meaning(field);
    descriptor->kind = ADS1299_SEMANTIC_NONE;
    descriptor->unit = NULL;
    descriptor->primary = 0.0;
    descriptor->secondary = 0.0;
    descriptor->primary_valid = 0u;
    descriptor->secondary_valid = 0u;
    descriptor->required_context_mask = 0u;
}

const char *ads1299_field_meaning(ads1299_field_id_t field) {
    switch (field) {
        case ADS1299_FIELD_ID_REV: return "Silicon revision code.";
        case ADS1299_FIELD_ID_DEVICE: return "Device-family identification code.";
        case ADS1299_FIELD_ID_CHANNEL_CODE: return "Physical channel-count code.";
        case ADS1299_FIELD_CONFIG1_DAISY_EN: return "Daisy-chain versus multiple-readback mode.";
        case ADS1299_FIELD_CONFIG1_CLK_EN: return "Internal oscillator output on the CLK pin.";
        case ADS1299_FIELD_CONFIG1_DR: return "Output data rate derived from fMOD = fCLK / 2.";
        case ADS1299_FIELD_CONFIG2_INT_CAL: return "External versus internally generated test signal.";
        case ADS1299_FIELD_CONFIG2_CAL_AMP: return "Internal calibration-signal amplitude relative to VREF span.";
        case ADS1299_FIELD_CONFIG2_CAL_FREQ: return "Internal calibration-signal frequency or DC mode.";
        case ADS1299_FIELD_CONFIG3_PD_REFBUF: return "Internal reference-buffer power state.";
        case ADS1299_FIELD_CONFIG3_BIAS_MEAS: return "Route BIAS_IN into the channel MUX path for measurement.";
        case ADS1299_FIELD_CONFIG3_BIASREF_INT: return "External versus internally generated BIASREF source.";
        case ADS1299_FIELD_CONFIG3_PD_BIAS: return "BIAS amplifier power state.";
        case ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS: return "BIAS lead-off sensing enable.";
        case ADS1299_FIELD_CONFIG3_BIAS_STAT: return "Read-only BIAS connection status.";
        case ADS1299_FIELD_LOFF_COMP_TH: return "Lead-off comparator positive/negative threshold pair.";
        case ADS1299_FIELD_LOFF_CURRENT: return "Lead-off excitation-current magnitude.";
        case ADS1299_FIELD_LOFF_FREQ: return "DC or AC lead-off excitation frequency.";
        case ADS1299_FIELD_CH_POWER_DOWN: return "Per-channel power state.";
        case ADS1299_FIELD_CH_GAIN: return "Per-channel PGA gain.";
        case ADS1299_FIELD_CH_SRB2: return "Per-channel SRB2 switch state.";
        case ADS1299_FIELD_CH_MUX: return "Per-channel input MUX selection.";
        case ADS1299_FIELD_BIAS_SENSP_MASK: return "Positive-input channel bitmap used in BIAS derivation.";
        case ADS1299_FIELD_BIAS_SENSN_MASK: return "Negative-input channel bitmap used in BIAS derivation.";
        case ADS1299_FIELD_LOFF_SENSP_MASK: return "Positive-input channel bitmap for lead-off detection.";
        case ADS1299_FIELD_LOFF_SENSN_MASK: return "Negative-input channel bitmap for lead-off detection.";
        case ADS1299_FIELD_LOFF_FLIP_MASK: return "Per-channel lead-off current-polarity flip bitmap.";
        case ADS1299_FIELD_LOFF_STATP_MASK: return "Read-only positive-electrode lead-off status bitmap.";
        case ADS1299_FIELD_LOFF_STATN_MASK: return "Read-only negative-electrode lead-off status bitmap.";
        case ADS1299_FIELD_GPIO_DATA: return "GPIO data bitmap for GPIO1 through GPIO4.";
        case ADS1299_FIELD_GPIO_DIRECTION: return "GPIO direction bitmap; 0=output and 1=input.";
        case ADS1299_FIELD_MISC1_SRB1: return "Global SRB1 switch to all inverting channel inputs.";
        case ADS1299_FIELD_CONFIG4_SINGLE_SHOT: return "Continuous versus single-shot conversion mode.";
        case ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE: return "Lead-off comparator enable state.";
        default: return NULL;
    }
}

ads1299_semantic_status_t ads1299_semantic_data_rate_sps(uint8_t dr_code,
                                                         double fclk_hz,
                                                         double *sps) {
    static const double divisors[7] = {
        128.0, 256.0, 512.0, 1024.0, 2048.0, 4096.0, 8192.0
    };
    if (!sps || dr_code > 6u || fclk_hz <= 0.0) return ADS1299_SEMANTIC_EINVAL;
    *sps = fclk_hz / divisors[dr_code];
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_test_amplitude_v(uint8_t amp_code,
                                                            double vref_span_v,
                                                            double *amplitude_v) {
    if (!amplitude_v || amp_code > 1u || vref_span_v <= 0.0) {
        return ADS1299_SEMANTIC_EINVAL;
    }
    *amplitude_v = (amp_code == 0u) ? (vref_span_v / 2400.0)
                                     : (vref_span_v / 1200.0);
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_test_frequency_hz(uint8_t freq_code,
                                                             double fclk_hz,
                                                             double *frequency_hz) {
    if (!frequency_hz || freq_code == 2u || freq_code > 3u) {
        return ADS1299_SEMANTIC_EINVAL;
    }
    if (freq_code == 3u) {
        *frequency_hz = 0.0;
        return ADS1299_SEMANTIC_OK;
    }
    if (fclk_hz <= 0.0) return ADS1299_SEMANTIC_EINVAL;
    *frequency_hz = (freq_code == 0u) ? (fclk_hz / 2097152.0)
                                      : (fclk_hz / 1048576.0);
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_biasref_internal_v(double avdd_v,
                                                              double avss_v,
                                                              double *biasref_v) {
    if (!biasref_v || avdd_v <= avss_v) return ADS1299_SEMANTIC_EINVAL;
    *biasref_v = (avdd_v + avss_v) / 2.0;
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_leadoff_threshold_pct(
    uint8_t threshold_code,
    double *positive_pct,
    double *negative_pct) {
    static const double positive[8] = {95.0, 92.5, 90.0, 87.5, 85.0, 80.0, 75.0, 70.0};
    static const double negative[8] = {5.0, 7.5, 10.0, 12.5, 15.0, 20.0, 25.0, 30.0};
    if (!positive_pct || !negative_pct || threshold_code > 7u) {
        return ADS1299_SEMANTIC_EINVAL;
    }
    *positive_pct = positive[threshold_code];
    *negative_pct = negative[threshold_code];
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_leadoff_current_a(uint8_t current_code,
                                                             double *current_a) {
    static const double currents[4] = {6e-9, 24e-9, 6e-6, 24e-6};
    if (!current_a || current_code > 3u) return ADS1299_SEMANTIC_EINVAL;
    *current_a = currents[current_code];
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_leadoff_frequency_hz(
    uint8_t frequency_code,
    double fclk_hz,
    double fdr_hz,
    uint8_t available_context_mask,
    double *frequency_hz,
    uint8_t *required_context_mask) {
    if (!frequency_hz || !required_context_mask || frequency_code > 3u) {
        return ADS1299_SEMANTIC_EINVAL;
    }
    *required_context_mask = 0u;
    if (frequency_code == 0u) {
        *frequency_hz = 0.0;
        return ADS1299_SEMANTIC_OK;
    }
    if (frequency_code == 1u || frequency_code == 2u) {
        if ((available_context_mask & ADS1299_SEMCTX_FCLK) == 0u) {
            *required_context_mask = ADS1299_SEMCTX_FCLK;
            return ADS1299_SEMANTIC_CONTEXT_REQUIRED;
        }
        if (fclk_hz <= 0.0) return ADS1299_SEMANTIC_EINVAL;
        *frequency_hz = (frequency_code == 1u) ? (fclk_hz / 262144.0)
                                               : (fclk_hz / 65536.0);
        return ADS1299_SEMANTIC_OK;
    }
    if ((available_context_mask & ADS1299_SEMCTX_FDR) == 0u) {
        *required_context_mask = ADS1299_SEMCTX_FDR;
        return ADS1299_SEMANTIC_CONTEXT_REQUIRED;
    }
    if (fdr_hz <= 0.0) return ADS1299_SEMANTIC_EINVAL;
    *frequency_hz = fdr_hz / 450.0;
    return ADS1299_SEMANTIC_OK;
}

ads1299_semantic_status_t ads1299_semantic_gain(uint8_t gain_code,
                                                double *gain_x) {
    static const double gains[7] = {1.0, 2.0, 4.0, 6.0, 8.0, 12.0, 24.0};
    if (!gain_x || gain_code > 6u) return ADS1299_SEMANTIC_EINVAL;
    *gain_x = gains[gain_code];
    return ADS1299_SEMANTIC_OK;
}

static const char *threshold_name(uint8_t code) {
    static const char *names[8] = {
        "95/5 percent", "92.5/7.5 percent", "90/10 percent", "87.5/12.5 percent",
        "85/15 percent", "80/20 percent", "75/25 percent", "70/30 percent"
    };
    return code < 8u ? names[code] : NULL;
}

static const char *gain_name(uint8_t code) {
    static const char *names[7] = {"Gain x1", "Gain x2", "Gain x4", "Gain x6",
                                   "Gain x8", "Gain x12", "Gain x24"};
    return code < 7u ? names[code] : NULL;
}

static const char *mux_name(uint8_t code) {
    static const char *names[8] = {
        "Normal electrode input", "Input shorted", "BIAS measurement",
        "MVDD supply measurement", "Temperature sensor", "Test signal",
        "BIAS_DRP", "BIAS_DRN"
    };
    return code < 8u ? names[code] : NULL;
}

static const char *data_rate_name(uint8_t code) {
    static const char *names[7] = {
        "fMOD/64", "fMOD/128", "fMOD/256", "fMOD/512",
        "fMOD/1024", "fMOD/2048", "fMOD/4096"
    };
    return code < 7u ? names[code] : NULL;
}

static const char *test_frequency_name(uint8_t code) {
    switch (code) {
        case 0u: return "fCLK/2^21";
        case 1u: return "fCLK/2^20";
        case 3u: return "DC";
        default: return NULL;
    }
}

static const char *leadoff_frequency_name(uint8_t code) {
    switch (code) {
        case 0u: return "DC";
        case 1u: return "fCLK/2^18";
        case 2u: return "fCLK/2^16";
        case 3u: return "fDR/450";
        default: return NULL;
    }
}

static const char *leadoff_current_name(uint8_t code) {
    switch (code) {
        case 0u: return "6 nA";
        case 1u: return "24 nA";
        case 2u: return "6 uA";
        case 3u: return "24 uA";
        default: return NULL;
    }
}

static const char *boolean_name(ads1299_field_id_t field, uint8_t code) {
    switch (field) {
        case ADS1299_FIELD_CONFIG1_DAISY_EN:
            return code ? "Multiple readback mode" : "Daisy-chain mode";
        case ADS1299_FIELD_CONFIG1_CLK_EN:
            return code ? "Clock output enabled" : "Clock output disabled";
        case ADS1299_FIELD_CONFIG2_INT_CAL:
            return code ? "Internal test source" : "External test source";
        case ADS1299_FIELD_CONFIG2_CAL_AMP:
            return code ? "2x calibration amplitude" : "1x calibration amplitude";
        case ADS1299_FIELD_CONFIG3_PD_REFBUF:
            return code ? "Reference buffer enabled" : "Reference buffer powered down";
        case ADS1299_FIELD_CONFIG3_BIAS_MEAS:
            return code ? "BIAS_IN routed to MUX" : "BIAS measurement path open";
        case ADS1299_FIELD_CONFIG3_BIASREF_INT:
            return code ? "Internal BIASREF" : "External BIASREF";
        case ADS1299_FIELD_CONFIG3_PD_BIAS:
            return code ? "BIAS amplifier enabled" : "BIAS amplifier powered down";
        case ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS:
            return code ? "BIAS sense enabled" : "BIAS sense disabled";
        case ADS1299_FIELD_CONFIG3_BIAS_STAT:
            return code ? "BIAS not connected" : "BIAS connected";
        case ADS1299_FIELD_CH_POWER_DOWN:
            return code ? "Channel powered down" : "Normal operation";
        case ADS1299_FIELD_CH_SRB2:
            return code ? "SRB2 closed" : "SRB2 open";
        case ADS1299_FIELD_MISC1_SRB1:
            return code ? "SRB1 switches closed" : "SRB1 switches open";
        case ADS1299_FIELD_CONFIG4_SINGLE_SHOT:
            return code ? "Single-shot mode" : "Continuous conversion mode";
        case ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE:
            return code ? "Lead-off comparators enabled" : "Lead-off comparators disabled";
        default:
            return code ? "Enabled" : "Disabled";
    }
}

ads1299_semantic_status_t ads1299_describe_field_code(
    ads1299_field_id_t field,
    uint8_t code,
    ads1299_variant_t variant,
    const ads1299_semantic_context_t *context,
    ads1299_code_descriptor_t *descriptor) {
    if (!descriptor || !ads1299_field_meaning(field) ||
        !ads1299_field_code_valid(field, code, variant)) {
        return ADS1299_SEMANTIC_EINVAL;
    }
    descriptor_reset(descriptor, field);

    switch (field) {
        case ADS1299_FIELD_ID_REV:
            descriptor->name = "Revision code";
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
        case ADS1299_FIELD_ID_DEVICE:
            descriptor->name = (code == ADS1299_ID_DEVICE_ADS1299) ?
                               "ADS1299 family" : "Device code";
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
        case ADS1299_FIELD_ID_CHANNEL_CODE:
            descriptor->kind = ADS1299_SEMANTIC_COUNT;
            descriptor->unit = "channels";
            descriptor->primary_valid = 1u;
            if (code == ADS1299_ID_CHANNEL_4) {
                descriptor->name = "4 channels";
                descriptor->primary = 4.0;
            } else if (code == ADS1299_ID_CHANNEL_6) {
                descriptor->name = "6 channels";
                descriptor->primary = 6.0;
            } else {
                descriptor->name = "8 channels";
                descriptor->primary = 8.0;
            }
            break;
        case ADS1299_FIELD_CONFIG1_DR:
            descriptor->name = data_rate_name(code);
            descriptor->kind = ADS1299_SEMANTIC_RATE;
            descriptor->unit = "SPS";
            if (!context_has(context, ADS1299_SEMCTX_FCLK)) {
                descriptor->required_context_mask = ADS1299_SEMCTX_FCLK;
            } else if (ads1299_semantic_data_rate_sps(code, context->fclk_hz,
                                                      &descriptor->primary) != ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            } else {
                descriptor->primary_valid = 1u;
            }
            break;
        case ADS1299_FIELD_CONFIG2_CAL_AMP:
            descriptor->name = boolean_name(field, code);
            descriptor->kind = ADS1299_SEMANTIC_VOLTAGE;
            descriptor->unit = "V";
            if (!context_has(context, ADS1299_SEMCTX_VREF_SPAN)) {
                descriptor->required_context_mask = ADS1299_SEMCTX_VREF_SPAN;
            } else if (ads1299_semantic_test_amplitude_v(code, context->vref_span_v,
                                                         &descriptor->primary) != ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            } else {
                descriptor->primary_valid = 1u;
            }
            break;
        case ADS1299_FIELD_CONFIG2_CAL_FREQ:
            descriptor->name = test_frequency_name(code);
            descriptor->kind = ADS1299_SEMANTIC_FREQUENCY;
            descriptor->unit = "Hz";
            if (code == 3u) {
                descriptor->primary = 0.0;
                descriptor->primary_valid = 1u;
            } else if (!context_has(context, ADS1299_SEMCTX_FCLK)) {
                descriptor->required_context_mask = ADS1299_SEMCTX_FCLK;
            } else if (ads1299_semantic_test_frequency_hz(code, context->fclk_hz,
                                                          &descriptor->primary) != ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            } else {
                descriptor->primary_valid = 1u;
            }
            break;
        case ADS1299_FIELD_CONFIG3_BIASREF_INT:
            descriptor->name = boolean_name(field, code);
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            if (code == 1u) {
                descriptor->kind = ADS1299_SEMANTIC_VOLTAGE;
                descriptor->unit = "V";
                if (!context_has(context, ADS1299_SEMCTX_AV_SUPPLY)) {
                    descriptor->required_context_mask = ADS1299_SEMCTX_AV_SUPPLY;
                } else if (ads1299_semantic_biasref_internal_v(context->avdd_v,
                                                               context->avss_v,
                                                               &descriptor->primary) != ADS1299_SEMANTIC_OK) {
                    return ADS1299_SEMANTIC_EINVAL;
                } else {
                    descriptor->primary_valid = 1u;
                }
            }
            break;
        case ADS1299_FIELD_LOFF_COMP_TH:
            descriptor->name = threshold_name(code);
            descriptor->kind = ADS1299_SEMANTIC_THRESHOLD_PAIR;
            descriptor->unit = "%";
            if (ads1299_semantic_leadoff_threshold_pct(code, &descriptor->primary,
                                                       &descriptor->secondary) != ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            }
            descriptor->primary_valid = 1u;
            descriptor->secondary_valid = 1u;
            break;
        case ADS1299_FIELD_LOFF_CURRENT:
            descriptor->name = leadoff_current_name(code);
            descriptor->kind = ADS1299_SEMANTIC_CURRENT;
            descriptor->unit = "A";
            if (ads1299_semantic_leadoff_current_a(code, &descriptor->primary) !=
                ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            }
            descriptor->primary_valid = 1u;
            break;
        case ADS1299_FIELD_LOFF_FREQ: {
            uint8_t required = 0u;
            const uint8_t available = context ? context->valid_mask : 0u;
            descriptor->name = leadoff_frequency_name(code);
            descriptor->kind = ADS1299_SEMANTIC_FREQUENCY;
            descriptor->unit = "Hz";
            const ads1299_semantic_status_t rc = ads1299_semantic_leadoff_frequency_hz(
                code, context ? context->fclk_hz : 0.0, context ? context->fdr_hz : 0.0,
                available, &descriptor->primary, &required);
            if (rc == ADS1299_SEMANTIC_EINVAL) return rc;
            if (rc == ADS1299_SEMANTIC_CONTEXT_REQUIRED) {
                descriptor->required_context_mask = required;
            } else {
                descriptor->primary_valid = 1u;
            }
            break;
        }
        case ADS1299_FIELD_CH_GAIN:
            descriptor->name = gain_name(code);
            descriptor->kind = ADS1299_SEMANTIC_GAIN;
            descriptor->unit = "x";
            if (ads1299_semantic_gain(code, &descriptor->primary) != ADS1299_SEMANTIC_OK) {
                return ADS1299_SEMANTIC_EINVAL;
            }
            descriptor->primary_valid = 1u;
            break;
        case ADS1299_FIELD_CH_MUX:
            descriptor->name = mux_name(code);
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
        case ADS1299_FIELD_BIAS_SENSP_MASK:
        case ADS1299_FIELD_BIAS_SENSN_MASK:
        case ADS1299_FIELD_LOFF_SENSP_MASK:
        case ADS1299_FIELD_LOFF_SENSN_MASK:
        case ADS1299_FIELD_LOFF_FLIP_MASK:
        case ADS1299_FIELD_LOFF_STATP_MASK:
        case ADS1299_FIELD_LOFF_STATN_MASK:
            descriptor->name = "Channel bitmap";
            descriptor->kind = ADS1299_SEMANTIC_CHANNEL_MASK;
            break;
        case ADS1299_FIELD_GPIO_DATA:
            descriptor->name = "GPIO data bitmap";
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
        case ADS1299_FIELD_GPIO_DIRECTION:
            descriptor->name = "GPIO direction bitmap";
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
        case ADS1299_FIELD_CONFIG1_DAISY_EN:
        case ADS1299_FIELD_CONFIG1_CLK_EN:
        case ADS1299_FIELD_CONFIG2_INT_CAL:
        case ADS1299_FIELD_CONFIG3_PD_REFBUF:
        case ADS1299_FIELD_CONFIG3_BIAS_MEAS:
        case ADS1299_FIELD_CONFIG3_PD_BIAS:
        case ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS:
        case ADS1299_FIELD_CONFIG3_BIAS_STAT:
        case ADS1299_FIELD_CH_POWER_DOWN:
        case ADS1299_FIELD_CH_SRB2:
        case ADS1299_FIELD_MISC1_SRB1:
        case ADS1299_FIELD_CONFIG4_SINGLE_SHOT:
        case ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE:
            descriptor->name = boolean_name(field, code);
            descriptor->kind = ADS1299_SEMANTIC_BOOLEAN;
            descriptor->primary = code ? 1.0 : 0.0;
            descriptor->primary_valid = 1u;
            break;
        default:
            descriptor->name = "Encoded value";
            descriptor->kind = ADS1299_SEMANTIC_ENUM;
            break;
    }
    return ADS1299_SEMANTIC_OK;
}
