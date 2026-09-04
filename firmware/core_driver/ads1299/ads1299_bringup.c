#include "ads1299.h"
#include "ads1299_runtime.h"

static uint8_t active_channel_count(const ads1299_t *dev) {
    if (dev && (dev->channel_count == 4u || dev->channel_count == 6u ||
                dev->channel_count == 8u)) {
        return dev->channel_count;
    }
    return ADS1299_CHANNEL_COUNT;
}

static int encode_config2_field(ads1299_field_id_t field,
                                uint8_t code,
                                uint8_t *value) {
    uint8_t next = 0u;
    if (!value || ads1299_field_encode(field, 0u, code, *value,
                                       ADS1299_VARIANT_8CH, &next) != 0) {
        return -1;
    }
    *value = next;
    return 0;
}

ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable) {
    if (!dev) return ADS1299_EINVAL;
    const ads1299_variant_t variant = ads1299_effective_variant(dev);
    /* TI DAISY_EN is inverted: 0=daisy-chain, 1=multiple readback. */
    return ads1299_safe_write_field(dev, ADS1299_FIELD_CONFIG1_DAISY_EN, 0u,
                                    enable ? 0u : 1u, variant, NULL);
}

ads1299_status_t ads1299_build_test_config2(
    const ads1299_test_signal_config_t *config, uint8_t *value) {
    if (!config || !value) return ADS1299_EINVAL;

    uint8_t built = ADS1299_RESET_CONFIG2;
    if (encode_config2_field(ADS1299_FIELD_CONFIG2_INT_CAL,
                             config->use_internal_source, &built) != 0 ||
        encode_config2_field(ADS1299_FIELD_CONFIG2_CAL_AMP,
                             config->amplitude_x2, &built) != 0 ||
        encode_config2_field(ADS1299_FIELD_CONFIG2_CAL_FREQ,
                             config->frequency_code, &built) != 0) {
        return ADS1299_EINVAL;
    }
    *value = built;
    return ADS1299_OK;
}

uint8_t ads1299_make_test_config2(const ads1299_test_signal_config_t *config) {
    uint8_t value = ADS1299_CONFIG2_RESERVED_BASE;
    if (ads1299_build_test_config2(config, &value) != ADS1299_OK) {
        return ADS1299_CONFIG2_RESERVED_BASE;
    }
    return value;
}

ads1299_status_t ads1299_configure_test_signal(
    ads1299_t *dev, const ads1299_test_signal_config_t *config) {
    if (!dev || !config) return ADS1299_EINVAL;
    uint8_t value = 0u;
    ads1299_status_t rc = ads1299_build_test_config2(config, &value);
    if (rc != ADS1299_OK) return rc;
    return ads1299_strict_write_register(dev, ADS1299_REG_CONFIG2, value,
                                         ads1299_effective_variant(dev));
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
