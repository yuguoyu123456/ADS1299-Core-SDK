#include "ads1299.h"

/*
 * Higher-level ADS1299 configuration and diagnostic helpers.
 *
 * This file deliberately keeps board/MCU concerns out of the device model.
 * Any platform that can implement ads1299_port_t gets the same SRB, BIAS,
 * lead-off, GPIO and identity behavior.
 */

static int valid_channel(uint8_t channel_1_to_8) {
    return channel_1_to_8 >= 1u && channel_1_to_8 <= ADS1299_CHANNEL_COUNT;
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
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_register_dump(ads1299_t *dev,
                                            ads1299_register_dump_t *dump) {
    if (!dev || !dump) return ADS1299_EINVAL;
    return ads1299_read_registers(dev,
                                  ADS1299_REG_ID,
                                  dump->value,
                                  ADS1299_REGISTER_COUNT);
}

ads1299_status_t ads1299_set_clock_output(ads1299_t *dev, int enable) {
    return ads1299_update_register_bits(dev,
                                        ADS1299_REG_CONFIG1,
                                        ADS1299_CONFIG1_CLK_EN,
                                        enable ? ADS1299_CONFIG1_CLK_EN : 0u);
}

ads1299_status_t ads1299_set_single_shot_mode(ads1299_t *dev, int enable) {
    return ads1299_update_register_bits(dev,
                                        ADS1299_REG_CONFIG4,
                                        ADS1299_CONFIG4_SINGLE_SHOT,
                                        enable ? ADS1299_CONFIG4_SINGLE_SHOT : 0u);
}

ads1299_status_t ads1299_power_down_channel(ads1299_t *dev,
                                            uint8_t channel_1_to_8) {
    if (!valid_channel(channel_1_to_8)) return ADS1299_EINVAL;

    /* TI recommends shorting a powered-down channel rather than leaving the
       channel input mux floating. */
    return ads1299_set_channel(dev,
                               channel_1_to_8,
                               ADS1299_GAIN_1,
                               ADS1299_MUX_SHORTED,
                               0,
                               1);
}

ads1299_status_t ads1299_set_srb1(ads1299_t *dev, int enable) {
    return ads1299_update_register_bits(dev,
                                        ADS1299_REG_MISC1,
                                        ADS1299_MISC1_SRB1,
                                        enable ? ADS1299_MISC1_SRB1 : 0u);
}

ads1299_status_t ads1299_set_channel_srb2(ads1299_t *dev,
                                          uint8_t channel_1_to_8,
                                          int enable) {
    if (!valid_channel(channel_1_to_8)) return ADS1299_EINVAL;
    const uint8_t reg = (uint8_t)(ADS1299_REG_CH1SET + channel_1_to_8 - 1u);
    return ads1299_update_register_bits(dev,
                                        reg,
                                        ADS1299_CH_SRB2,
                                        enable ? ADS1299_CH_SRB2 : 0u);
}

ads1299_status_t ads1299_configure_bias(ads1299_t *dev,
                                        const ads1299_bias_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;

    uint8_t previous_config3 = 0;
    ads1299_status_t rc = ads1299_read_register(dev,
                                                ADS1299_REG_CONFIG3,
                                                &previous_config3);
    if (rc != ADS1299_OK) return rc;

    /* Program sense masks before enabling the amplifier so the feedback path
       is deterministic at the moment the BIAS buffer is enabled. */
    rc = ads1299_write_register(dev,
                                ADS1299_REG_BIAS_SENSP,
                                config->positive_sense_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_write_register(dev,
                                ADS1299_REG_BIAS_SENSN,
                                config->negative_sense_mask);
    if (rc != ADS1299_OK) return rc;

    uint8_t config3 = ADS1299_CONFIG3_RESERVED_BASE;
    if (config->enable_reference_buffer) {
        config3 |= ADS1299_CONFIG3_REFBUF_ENABLE;
    }
    if (config->enable_bias_measurement) {
        config3 |= ADS1299_CONFIG3_BIAS_MEAS;
    }
    if (config->use_internal_bias_reference) {
        config3 |= ADS1299_CONFIG3_BIASREF_INT;
    }
    if (config->enable_bias_buffer) {
        config3 |= ADS1299_CONFIG3_BIASBUF_ENABLE;
    }
    if (config->enable_bias_leadoff_sense) {
        config3 |= ADS1299_CONFIG3_BIAS_LOFF_SENS;
    }

    rc = ads1299_write_register(dev, ADS1299_REG_CONFIG3, config3);
    if (rc != ADS1299_OK) return rc;

    /* ADS1299 datasheet specifies up to 150 ms reference start-up. Only pay
       that delay when the helper actually transitions the reference buffer
       from disabled to enabled. */
    if (config->enable_reference_buffer &&
        !(previous_config3 & ADS1299_CONFIG3_REFBUF_ENABLE)) {
        dev->port.delay_us(dev->port.user, 150000u);
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_read_bias_status(ads1299_t *dev,
                                          uint8_t *bias_off) {
    if (!dev || !bias_off) return ADS1299_EINVAL;

    uint8_t config3 = 0;
    ads1299_status_t rc = ads1299_read_register(dev,
                                                ADS1299_REG_CONFIG3,
                                                &config3);
    if (rc != ADS1299_OK) return rc;
    *bias_off = (uint8_t)((config3 & ADS1299_CONFIG3_BIAS_STAT) ? 1u : 0u);
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_lead_off(ads1299_t *dev,
                                            const ads1299_leadoff_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;
    if (config->comparator_threshold_code > 7u ||
        config->current_code > 3u ||
        config->frequency_code > 3u) {
        return ADS1299_EINVAL;
    }

    const uint8_t loff = (uint8_t)(
        ((config->comparator_threshold_code << ADS1299_LOFF_COMP_TH_SHIFT) &
         ADS1299_LOFF_COMP_TH_MASK) |
        ((config->current_code << ADS1299_LOFF_CURRENT_SHIFT) &
         ADS1299_LOFF_CURRENT_MASK) |
        (config->frequency_code & ADS1299_LOFF_FREQ_MASK));

    ads1299_status_t rc = ads1299_write_register(dev, ADS1299_REG_LOFF, loff);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_write_register(dev,
                                ADS1299_REG_LOFF_SENSP,
                                config->positive_sense_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_write_register(dev,
                                ADS1299_REG_LOFF_SENSN,
                                config->negative_sense_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_write_register(dev,
                                ADS1299_REG_LOFF_FLIP,
                                config->current_flip_mask);
    if (rc != ADS1299_OK) return rc;

    return ads1299_update_register_bits(
        dev,
        ADS1299_REG_CONFIG4,
        ADS1299_CONFIG4_LOFF_COMP_ENABLE,
        config->enable_comparators ? ADS1299_CONFIG4_LOFF_COMP_ENABLE : 0u);
}

ads1299_status_t ads1299_read_lead_off_status(ads1299_t *dev,
                                              ads1299_leadoff_status_t *status) {
    if (!dev || !status) return ADS1299_EINVAL;

    ads1299_status_t rc = ads1299_read_register(dev,
                                                ADS1299_REG_LOFF_STATP,
                                                &status->positive_off_mask);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_read_register(dev,
                               ADS1299_REG_LOFF_STATN,
                               &status->negative_off_mask);
    if (rc != ADS1299_OK) return rc;
    return ads1299_read_bias_status(dev, &status->bias_off);
}

ads1299_status_t ads1299_gpio_configure(ads1299_t *dev,
                                         uint8_t direction_mask,
                                         uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    const uint8_t value = (uint8_t)(
        ((output_value_mask & 0x0Fu) << ADS1299_GPIO_DATA_SHIFT) |
        (direction_mask & ADS1299_GPIO_DIRECTION_MASK));
    return ads1299_write_register(dev, ADS1299_REG_GPIO, value);
}

ads1299_status_t ads1299_gpio_write(ads1299_t *dev,
                                    uint8_t output_value_mask) {
    if (!dev) return ADS1299_EINVAL;
    return ads1299_update_register_bits(
        dev,
        ADS1299_REG_GPIO,
        ADS1299_GPIO_DATA_MASK,
        (uint8_t)((output_value_mask & 0x0Fu) << ADS1299_GPIO_DATA_SHIFT));
}

ads1299_status_t ads1299_gpio_read(ads1299_t *dev,
                                   uint8_t *pin_state_mask) {
    if (!dev || !pin_state_mask) return ADS1299_EINVAL;

    uint8_t value = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_GPIO, &value);
    if (rc != ADS1299_OK) return rc;
    *pin_state_mask = (uint8_t)((value & ADS1299_GPIO_DATA_MASK) >>
                               ADS1299_GPIO_DATA_SHIFT);
    return ADS1299_OK;
}

double ads1299_code_to_microvolts(int32_t code,
                                  double vref_volts,
                                  double gain) {
    return ads1299_code_to_volts(code, vref_volts, gain) * 1000000.0;
}
