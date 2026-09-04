#include "ads1299_runtime.h"

#define ADS1299_WAKEUP_GUARD_US 4u
#define ADS1299_START_PULSE_US 2u

ads1299_status_t ads1299_wakeup(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_WAKEUP);
    if (rc == ADS1299_OK) {
        dev->port.delay_us(dev->port.user, ADS1299_WAKEUP_GUARD_US);
        dev->standby_mode = 0u;
    }
    return rc;
}

ads1299_status_t ads1299_standby(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_STANDBY);
    if (rc == ADS1299_OK) {
        dev->standby_mode = 1u;
        dev->continuous_mode = 0u;
    }
    return rc;
}

ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down) {
    if (!dev || !dev->port.pwdn_write) return ADS1299_EINVAL;
    dev->port.pwdn_write(dev->port.user, power_down ? 0 : 1);
    if (power_down) {
        dev->continuous_mode = 0u;
        dev->standby_mode = 0u;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_start_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write || !dev->port.delay_us) return ADS1299_EINVAL;
    if (dev->standby_mode) return ADS1299_ESTATE;
    dev->port.start_write(dev->port.user, 1);
    dev->port.delay_us(dev->port.user, ADS1299_START_PULSE_US);
    return ADS1299_OK;
}

ads1299_status_t ads1299_stop_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write) return ADS1299_EINVAL;
    if (dev->standby_mode) return ADS1299_ESTATE;
    dev->port.start_write(dev->port.user, 0);
    return ADS1299_OK;
}

ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us) {
    if (!dev || !dev->port.drdy_read || !dev->port.delay_us || timeout_us == 0u) {
        return ADS1299_EINVAL;
    }
    if (dev->standby_mode) return ADS1299_ESTATE;
    if (poll_interval_us == 0u) poll_interval_us = 1u;

    uint32_t elapsed = 0u;
    while (elapsed < timeout_us) {
        if (dev->port.drdy_read(dev->port.user) == 0) return ADS1299_OK;
        uint32_t step = poll_interval_us;
        if (step > timeout_us - elapsed) step = timeout_us - elapsed;
        dev->port.delay_us(dev->port.user, step);
        elapsed += step;
    }
    return ADS1299_ETIMEOUT;
}

ads1299_variant_t ads1299_effective_variant(const ads1299_t *dev) {
    if (dev) {
        if (dev->channel_count == 4u) return ADS1299_VARIANT_4CH;
        if (dev->channel_count == 6u) return ADS1299_VARIANT_6CH;
        if (dev->channel_count == 8u) return ADS1299_VARIANT_8CH;
    }
    return ADS1299_VARIANT_8CH;
}

ads1299_status_t ads1299_read_field(ads1299_t *dev,
                                    ads1299_field_id_t field,
                                    uint8_t channel_1_to_8,
                                    ads1299_variant_t variant,
                                    uint8_t *code) {
    if (!dev || !code || ads1299_variant_channel_mask(variant) == 0u) {
        return ADS1299_EINVAL;
    }

    uint8_t address = 0u;
    if (ads1299_field_register_address(field, channel_1_to_8, variant, &address) != 0) {
        return ADS1299_EINVAL;
    }

    uint8_t register_value = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &register_value);
    if (rc != ADS1299_OK) return rc;

    uint8_t decoded = 0u;
    if (ads1299_field_decode(field, register_value, &decoded) != 0) {
        return ADS1299_EINVAL;
    }
    if (!ads1299_field_code_valid(field, decoded, variant)) {
        return ADS1299_EVERIFY;
    }
    *code = decoded;
    return ADS1299_OK;
}

ads1299_status_t ads1299_safe_write_registers(ads1299_t *dev,
                                              uint8_t address,
                                              const uint8_t *requested,
                                              size_t count,
                                              ads1299_variant_t variant,
                                              uint8_t *written_values) {
    if (!dev || !requested || count == 0u || address > ADS1299_REG_LAST ||
        count > (size_t)(ADS1299_REG_LAST - address + 1u) ||
        count > ADS1299_REGISTER_COUNT) {
        return ADS1299_EINVAL;
    }

    uint8_t sanitized[ADS1299_REGISTER_COUNT] = {0};
    for (size_t i = 0; i < count; ++i) {
        if (ads1299_sanitize_register_write((uint8_t)(address + i), requested[i],
                                            variant, &sanitized[i]) != 0) {
            return ADS1299_EINVAL;
        }
    }

    ads1299_status_t rc = ads1299_write_registers(dev, address, sanitized, count);
    if (rc == ADS1299_OK && written_values) {
        for (size_t i = 0; i < count; ++i) written_values[i] = sanitized[i];
    }
    return rc;
}

ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value) {
    return ads1299_safe_write_registers(dev, address, &requested, 1u, variant,
                                        written_value);
}

ads1299_status_t ads1299_strict_write_registers(ads1299_t *dev,
                                                uint8_t address,
                                                const uint8_t *values,
                                                size_t count,
                                                ads1299_variant_t variant) {
    if (!dev || !values || count == 0u || address > ADS1299_REG_LAST ||
        count > (size_t)(ADS1299_REG_LAST - address + 1u) ||
        count > ADS1299_REGISTER_COUNT) {
        return ADS1299_EINVAL;
    }

    /* Validate the complete sequence before any bus activity. */
    for (size_t i = 0; i < count; ++i) {
        if (!ads1299_register_write_value_valid((uint8_t)(address + i), values[i],
                                                variant)) {
            return ADS1299_EINVAL;
        }
    }
    return ads1299_write_registers(dev, address, values, count);
}

ads1299_status_t ads1299_strict_write_register(ads1299_t *dev,
                                               uint8_t address,
                                               uint8_t value,
                                               ads1299_variant_t variant) {
    return ads1299_strict_write_registers(dev, address, &value, 1u, variant);
}

ads1299_status_t ads1299_safe_update_register_bits(ads1299_t *dev,
                                                   uint8_t address,
                                                   uint8_t mask,
                                                   uint8_t value,
                                                   ads1299_variant_t variant,
                                                   uint8_t *written_value) {
    if (!dev || mask == 0u) return ADS1299_EINVAL;
    const ads1299_register_info_t *info = ads1299_register_info(address);
    if (!info || info->read_only || !ads1299_register_available(address, variant) ||
        (mask & (uint8_t)~info->writable_mask) != 0u) {
        return ADS1299_EINVAL;
    }

    uint8_t current = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    const uint8_t requested = (uint8_t)((current & (uint8_t)~mask) | (value & mask));
    return ads1299_safe_write_register(dev, address, requested, variant, written_value);
}

ads1299_status_t ads1299_safe_write_field(ads1299_t *dev,
                                          ads1299_field_id_t field,
                                          uint8_t channel_1_to_8,
                                          uint8_t code,
                                          ads1299_variant_t variant,
                                          uint8_t *written_register_value) {
    if (!dev) return ADS1299_EINVAL;

    uint8_t address = 0u;
    if (ads1299_field_register_address(field, channel_1_to_8, variant, &address) != 0 ||
        !ads1299_field_code_valid(field, code, variant)) {
        return ADS1299_EINVAL;
    }
    const ads1299_field_info_t *info = ads1299_field_info(field);
    if (!info || !info->writable) return ADS1299_EINVAL;

    uint8_t current = 0u;
    ads1299_status_t rc = ads1299_read_register(dev, address, &current);
    if (rc != ADS1299_OK) return rc;

    uint8_t requested = 0u;
    if (ads1299_field_encode(field, channel_1_to_8, code, current, variant,
                             &requested) != 0) {
        return ADS1299_EINVAL;
    }
    return ads1299_safe_write_register(dev, address, requested, variant,
                                       written_register_value);
}
