#include "ads1299.h"

#define ADS1299_WAKEUP_GUARD_US 4u
#define ADS1299_START_PULSE_US 2u

ads1299_status_t ads1299_wakeup(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_command(dev, ADS1299_CMD_WAKEUP);
    if (rc == ADS1299_OK) {
        dev->port.delay_us(dev->port.user, ADS1299_WAKEUP_GUARD_US);
    }
    return rc;
}

ads1299_status_t ads1299_standby(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;
    return ads1299_command(dev, ADS1299_CMD_STANDBY);
}

ads1299_status_t ads1299_set_power_down(ads1299_t *dev, int power_down) {
    if (!dev || !dev->port.pwdn_write) return ADS1299_EINVAL;
    dev->port.pwdn_write(dev->port.user, power_down ? 0 : 1);
    if (power_down) {
        dev->continuous_mode = 0u;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_start_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write || !dev->port.delay_us) return ADS1299_EINVAL;
    dev->port.start_write(dev->port.user, 1);
    /* TI requires START high for at least 2 tCLK. A 2-us guard is conservative
       for the nominal 2.048-MHz device clock while keeping this API portable. */
    dev->port.delay_us(dev->port.user, ADS1299_START_PULSE_US);
    return ADS1299_OK;
}

ads1299_status_t ads1299_stop_pin(ads1299_t *dev) {
    if (!dev || !dev->port.start_write) return ADS1299_EINVAL;
    dev->port.start_write(dev->port.user, 0);
    return ADS1299_OK;
}

ads1299_status_t ads1299_wait_drdy(ads1299_t *dev,
                                   uint32_t timeout_us,
                                   uint32_t poll_interval_us) {
    if (!dev || !dev->port.drdy_read || !dev->port.delay_us || timeout_us == 0u) {
        return ADS1299_EINVAL;
    }
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

ads1299_status_t ads1299_safe_write_register(ads1299_t *dev,
                                             uint8_t address,
                                             uint8_t requested,
                                             ads1299_variant_t variant,
                                             uint8_t *written_value) {
    if (!dev) return ADS1299_EINVAL;
    uint8_t sanitized = 0u;
    if (ads1299_sanitize_register_write(address, requested, variant, &sanitized) != 0) {
        return ADS1299_EINVAL;
    }
    ads1299_status_t rc = ads1299_write_register(dev, address, sanitized);
    if (rc == ADS1299_OK && written_value) *written_value = sanitized;
    return rc;
}
