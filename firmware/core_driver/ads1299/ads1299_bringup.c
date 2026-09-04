#include "ads1299.h"

static uint8_t active_channel_count(const ads1299_t *dev) {
    if (dev && (dev->channel_count == 4u || dev->channel_count == 6u ||
                dev->channel_count == 8u)) {
        return dev->channel_count;
    }
    return ADS1299_CHANNEL_COUNT;
}

ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable) {
    uint8_t config1 = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_CONFIG1, &config1);
    if (rc != ADS1299_OK) return rc;

    if (enable) {
        config1 &= (uint8_t)~ADS1299_CONFIG1_DAISY_EN;
    } else {
        config1 |= ADS1299_CONFIG1_DAISY_EN;
    }
    return ads1299_write_register(dev, ADS1299_REG_CONFIG1, config1);
}

uint8_t ads1299_make_test_config2(const ads1299_test_signal_config_t *config) {
    if (!config) return ADS1299_CONFIG2_RESERVED_BASE;
    uint8_t value = (uint8_t)(ADS1299_CONFIG2_RESERVED_BASE |
                              (config->frequency_code & ADS1299_CONFIG2_CAL_FREQ_MASK));
    if (config->use_internal_source) value |= ADS1299_CONFIG2_INT_CAL;
    if (config->amplitude_x2) value |= ADS1299_CONFIG2_CAL_AMP;
    return value;
}

ads1299_status_t ads1299_configure_test_signal(
    ads1299_t *dev, const ads1299_test_signal_config_t *config) {
    if (!dev || !config || config->frequency_code > ADS1299_TEST_FREQ_DC ||
        config->frequency_code == ADS1299_TEST_FREQ_RESERVED) {
        return ADS1299_EINVAL;
    }
    return ads1299_write_register(dev, ADS1299_REG_CONFIG2,
                                  ads1299_make_test_config2(config));
}

uint8_t ads1299_make_internal_test_config2(int amplitude_x2, uint8_t freq_code) {
    const ads1299_test_signal_config_t config = {
        .use_internal_source = 1u,
        .amplitude_x2 = amplitude_x2 ? 1u : 0u,
        .frequency_code = freq_code,
    };
    return ads1299_make_test_config2(&config);
}

ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code) {
    const ads1299_test_signal_config_t config = {
        .use_internal_source = 1u,
        .amplitude_x2 = amplitude_x2 ? 1u : 0u,
        .frequency_code = freq_code,
    };
    ads1299_status_t rc = ads1299_configure_test_signal(dev, &config);
    if (rc != ADS1299_OK) return rc;

    const uint8_t count = active_channel_count(dev);
    for (uint8_t ch = 1; ch <= count; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_TEST, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code) {
    if (!dev) return ADS1299_EINVAL;
    ads1299_status_t rc;
    const uint8_t count = active_channel_count(dev);
    for (uint8_t ch = 1; ch <= count; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_SHORTED, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}
