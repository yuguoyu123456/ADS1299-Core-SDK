#include "ads1299.h"

ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable) {
    uint8_t config1 = 0;
    ads1299_status_t rc = ads1299_read_register(dev, ADS1299_REG_CONFIG1, &config1);
    if (rc != ADS1299_OK) return rc;

    /* TI names bit 6 DAISY_EN, but 0 selects daisy-chain mode and 1 selects
       multiple-readback mode. Preserve every other CONFIG1 bit. */
    if (enable) {
        config1 &= (uint8_t)~ADS1299_CONFIG1_DAISY_EN;
    } else {
        config1 |= ADS1299_CONFIG1_DAISY_EN;
    }
    return ads1299_write_register(dev, ADS1299_REG_CONFIG1, config1);
}

uint8_t ads1299_make_internal_test_config2(int amplitude_x2, uint8_t freq_code) {
    uint8_t value = (uint8_t)(ADS1299_CONFIG2_RESERVED_BASE |
                              ADS1299_CONFIG2_INT_CAL |
                              (freq_code & ADS1299_CONFIG2_CAL_FREQ_MASK));
    if (amplitude_x2) value |= ADS1299_CONFIG2_CAL_AMP;
    return value;
}

ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code) {
    if (freq_code == ADS1299_TEST_FREQ_RESERVED || freq_code > ADS1299_TEST_FREQ_DC) {
        return ADS1299_EINVAL;
    }

    const uint8_t config2 = ads1299_make_internal_test_config2(amplitude_x2, freq_code);
    ads1299_status_t rc = ads1299_write_register(dev, ADS1299_REG_CONFIG2, config2);
    if (rc != ADS1299_OK) return rc;

    for (uint8_t ch = 1; ch <= ADS1299_CHANNEL_COUNT; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_TEST, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code) {
    ads1299_status_t rc;
    for (uint8_t ch = 1; ch <= ADS1299_CHANNEL_COUNT; ++ch) {
        rc = ads1299_set_channel(dev, ch, gain_code, ADS1299_MUX_SHORTED, 0, 0);
        if (rc != ADS1299_OK) return rc;
    }
    return ADS1299_OK;
}
