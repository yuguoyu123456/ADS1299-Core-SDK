#include "ads1299.h"
#include "ads1299_runtime.h"

static uint8_t device_channel_mask(const ads1299_t *dev) {
    return ads1299_variant_channel_mask(ads1299_effective_variant(dev));
}

static int valid_channel(const ads1299_t *dev, uint8_t channel_1_to_8) {
    if (!dev || channel_1_to_8 < 1u || channel_1_to_8 > 8u) return 0;
    const uint8_t mask = device_channel_mask(dev);
    return (mask & ADS1299_CHANNEL_MASK(channel_1_to_8)) != 0u;
}

static int encode_field(ads1299_field_id_t field,
                        uint8_t channel_1_to_8,
                        uint8_t code,
                        ads1299_variant_t variant,
                        uint8_t *register_value) {
    uint8_t next = 0u;
    if (!register_value ||
        ads1299_field_encode(field, channel_1_to_8, code, *register_value,
                             variant, &next) != 0) {
        return -1;
    }
    *register_value = next;
    return 0;
}

static int gain_bits_to_field_code(uint8_t gain_bits, uint8_t *field_code) {
    if (!field_code || (gain_bits & (uint8_t)~ADS1299_CH_GAIN_MASK) != 0u) return -1;
    const uint8_t code = (uint8_t)((gain_bits & ADS1299_CH_GAIN_MASK) >> 4u);
    if (!ads1299_field_code_valid(ADS1299_FIELD_CH_GAIN, code,
                                  ADS1299_VARIANT_8CH)) {
        return -1;
    }
    *field_code = code;
    return 0;
}

ads1299_status_t ads1299_update_register_bits(ads1299_t *dev,
                                              uint8_t address,
                                              uint8_t mask,
                                              uint8_t value) {
    if (!dev || address > ADS1299_REG_LAST) return ADS1299_EINVAL;

    uint8_t current = 0;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    const uint8_t next = (uint8_t)((current & (uint8_t)~mask) | (value & mask));
    if (next == current) return ADS1299_OK;
    return ads1299_write_register(dev, address, next);
}

ads1299_status_t ads1299_verify_register(ads1299_t *dev,
                                         uint8_t address,
                                         uint8_t expected,
                                         uint8_t mask,
                                         uint8_t *actual) {
    if (!dev || address > ADS1299_REG_LAST) return ADS1299_EINVAL;

    uint8_t value = 0;
    ads1299_status_t rc = ads1299_read_register(dev, address, &value);
    if (actual) *actual = value;
    if (rc != ADS1299_OK) return rc;

    return ((value & mask) == (expected & mask)) ? ADS1299_OK : ADS1299_EVERIFY;
}

ads1299_status_t ads1299_read_device_id(ads1299_t *dev,
                                        ads1299_device_id_t *id) {
    if (!dev || !id) return ADS1299_EINVAL;

    uint8_t raw = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_ID, &raw);
    if (rc != ADS1299_OK) return rc;

    id->raw = raw;
    id->revision = (uint8_t)((raw & ADS1299_ID_REV_MASK) >> ADS1299_ID_REV_SHIFT);
    id->device_code = (uint8_t)((raw & ADS1299_ID_DEVICE_MASK) >> ADS1299_ID_DEVICE_SHIFT);
    id->channel_code = (uint8_t)(raw & ADS1299_ID_CHANNEL_MASK);
    id->is_ads1299_family = (uint8_t)(
        ((raw & ADS1299_ID_RESERVED_ONE) != 0u) &&
        (id->device_code == ADS1299_ID_DEVICE_ADS1299));

    switch (id->channel_code) {
        case ADS1299_ID_CHANNEL_4: id->channel_count = 4u; break;
        case ADS1299_ID_CHANNEL_6: id->channel_count = 6u; break;
        case ADS1299_ID_CHANNEL_8: id->channel_count = 8u; break;
        default: id->channel_count = 0u; break;
    }

    dev->channel_count = id->is_ads1299_family ? id->channel_count : 0u;
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_register_dump(ads1299_t *dev,
                                            ads1299_register_dump_t *dump) {
    if (!dev || !dump) return ADS1299_EINVAL;
    return ads1299_read_registers(dev, ADS1299_REG_ID, dump->value,
                                  ADS1299_REGISTER_COUNT);
}

ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_DR, 0u, dr_code,
                                    variant, NULL);
}

ads1299_status_t ads1299_set_clock_output(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_CLK_EN, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_single_shot_mode(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG4_SINGLE_SHOT, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_channel(ads1299_t *dev,
                                     uint8_t channel_1_to_8,
                                     uint8_t gain_code,
                                     uint8_t mux_code,
                                     int srb2,
                                     int power_down) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;

    uint8_t gain_field = 0u;
    if (gain_bits_to_field_code(gain_code, &gain_field) != 0) return ADS1299_EINVAL;

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    if (!ads1299_field_code_valid(ADS1299_FIELD_CH_MUX, mux_code, variant)) {
        return ADS1299_EINVAL;
    }

    uint8_t address = 0u;
    if (ads1299_field_register_address(ADS1299_FIELD_CH_GAIN, channel_1_to_8,
                                       variant, &address) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t value = ADS1299_RESET_CHNSET;
    if (encode_field(ADS1299_FIELD_CH_POWER_DOWN, channel_1_to_8,
                     power_down ? 1u : 0u, variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_GAIN, channel_1_to_8, gain_field,
                     variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_SRB2, channel_1_to_8,
                     srb2 ? 1u : 0u, variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_CH_MUX, channel_1_to_8, mux_code,
                     variant, &value) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_strict_write_register(dev, address, value, variant);
}

ads1299_status_t ads1299_power_down_channel(ads1299_t *dev,
                                            uint8_t channel_1_to_8) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;
    return ads1299_set_channel(dev, channel_1_to_8, ADS1299_GAIN_1,
                               ADS1299_MUX_SHORTED, 0, 1);
}

ads1299_status_t ads1299_set_srb1(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_MISC1_SRB1, 0u,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_set_channel_srb2(ads1299_t *dev,
                                          uint8_t channel_1_to_8,
                                          int enable) {
    if (!valid_channel(dev, channel_1_to_8)) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CH_SRB2, channel_1_to_8,
                                    enable ? 1u : 0u, variant, NULL);
}

ads1299_status_t ads1299_configure_bias(ads1299_t *dev,
                                        const ads1299_bias_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;

    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    const uint8_t chmask = ads1299_variant_channel_mask(variant);
    const uint8_t positive_mask = (uint8_t)(config->positive_sense_mask & chmask);
    const uint8_t negative_mask = (uint8_t)(config->negative_sense_mask & chmask);

    uint8_t config3 = ADS1299_RESET_CONFIG3;
    if (encode_field(ADS1299_FIELD_CONFIG3_PD_REFBUF, 0u,
                     config->enable_reference_buffer ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIAS_MEAS, 0u,
                     config->enable_bias_measurement ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIASREF_INT, 0u,
                     config->use_internal_bias_reference ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_PD_BIAS, 0u,
                     config->enable_bias_buffer ? 1u : 0u,
                     variant, &config3) != 0 ||
        encode_field(ADS1299_FIELD_CONFIG3_BIAS_LOFF_SENS, 0u,
                     config->enable_bias_leadoff_sense ? 1u : 0u,
                     variant, &config3) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t previous_config3 = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_CONFIG3,
                                                &previous_config3);
    if (rc != ADS1299_OK) return rc;

    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_BIAS_SENSP_MASK, 0u,
                                  positive_mask, variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_BIAS_SENSN_MASK, 0u,
                                  negative_mask, variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_strict_write_register(dev, ADS1299_REG_CONFIG3, config3, variant);
    if (rc != ADS1299_OK) return rc;

    if (config->enable_reference_buffer &&
        !(previous_config3 & ADS1299_CONFIG3_REFBUF_ENABLE)) {
        dev->port.delay_us(dev->port.user, 150000u);
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_bias_status(ads1299_t *dev,
                                          uint8_t *bias_off) {
    if (!dev || !bias_off) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_read_field(dev, ADS1299_FIELD_CONFIG3_BIAS_STAT, 0u,
                              variant, bias_off);
}

ads1299_status_t ads1299_configure_lead_off(ads1299_t *dev,
                                            const ads1299_leadoff_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    if (!ads1299_field_code_valid(ADS1299_FIELD_LOFF_COMP_TH,
                                  config->comparator_threshold_code, variant) ||
        !ads1299_field_code_valid(ADS1299_FIELD_LOFF_CURRENT,
                                  config->current_code, variant) ||
        !ads1299_field_code_valid(ADS1299_FIELD_LOFF_FREQ,
                                  config->frequency_code, variant)) {
        return ADS1299_EINVAL;
    }

    uint8_t loff = ADS1299_RESET_LOFF;
    if (encode_field(ADS1299_FIELD_LOFF_COMP_TH, 0u,
                     config->comparator_threshold_code, variant, &loff) != 0 ||
        encode_field(ADS1299_FIELD_LOFF_CURRENT, 0u,
                     config->current_code, variant, &loff) != 0 ||
        encode_field(ADS1299_FIELD_LOFF_FREQ, 0u,
                     config->frequency_code, variant, &loff) != 0) {
        return ADS1299_EINVAL;
    }

    const uint8_t chmask = ads1299_variant_channel_mask(variant);
    ads1299_status_t rc = ads1299_strict_write_register(dev, ADS1299_REG_LOFF,
                                                        loff, variant);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_SENSP_MASK, 0u,
                                  (uint8_t)(config->positive_sense_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_SENSN_MASK, 0u,
                                  (uint8_t)(config->negative_sense_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_safe_write_field(dev, ADS1299_FIELD_LOFF_FLIP_MASK, 0u,
                                  (uint8_t)(config->current_flip_mask & chmask),
                                  variant, NULL);
    if (rc != ADS1299_OK) return rc;
    return ads1299_safe_write_field(dev,
                                    ADS1299_FIELD_CONFIG4_LOFF_COMP_ENABLE,
                                    0u, config->enable_comparators ? 1u : 0u,
                                    variant, NULL);
}

ads1299_status_t ads1299_read_lead_off_status(ads1299_t *dev,
                                              ads1299_leadoff_status_t *status) {
    if (!dev || !status) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    ads1299_status_t rc = ads1299_read_field(dev, ADS1299_FIELD_LOFF_STATP_MASK,
                                             0u, variant,
                                             &status->positive_off_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_read_field(dev, ADS1299_FIELD_LOFF_STATN_MASK, 0u, variant,
                            &status->negative_off_mask);
    if (rc != ADS1299_OK) return rc;
    return ads1299_read_bias_status(dev, &status->bias_off);
}

ads1299_status_t ads1299_gpio_configure(ads1299_t *dev,
                                        uint8_t direction_mask,
                                        uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    uint8_t value = ADS1299_RESET_GPIO;
    if (encode_field(ADS1299_FIELD_GPIO_DATA, 0u,
                     (uint8_t)(output_value_mask & 0x0Fu), variant, &value) != 0 ||
        encode_field(ADS1299_FIELD_GPIO_DIRECTION, 0u,
                     (uint8_t)(direction_mask & 0x0Fu), variant, &value) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_strict_write_register(dev, ADS1299_REG_GPIO, value, variant);
}

ads1299_status_t ads1299_gpio_write(ads1299_t *dev,
                                    uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_safe_write_field(dev, ADS1299_FIELD_GPIO_DATA, 0u,
                                    (uint8_t)(output_value_mask & 0x0Fu),
                                    variant, NULL);
}

ads1299_status_t ads1299_gpio_read(ads1299_t *dev,
                                   uint8_t *pin_state_mask) {
    if (!dev || !pin_state_mask) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    return ads1299_read_field(dev, ADS1299_FIELD_GPIO_DATA, 0u, variant,
                              pin_state_mask);
}

double ads1299_code_to_microvolts(int32_t code,
                                  double vref_volts,
                                  double gain) {
    return ads1299_code_to_volts(code, vref_volts, gain) * 1000000.0;
}
