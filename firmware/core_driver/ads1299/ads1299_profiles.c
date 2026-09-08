#include "ads1299_profiles.h"

static uint8_t active_channel_mask(const ads1299_t *dev) {
    uint8_t count = dev ? dev->channel_count : 0u;
    if (count == 0u || count > ADS1299_CHANNEL_COUNT) count = ADS1299_CHANNEL_COUNT;
    if (count >= 8u) return 0xFFu;
    return (uint8_t)((1u << count) - 1u);
}

static ads1299_eeg_profile_t make_profile(uint8_t dr_code) {
    ads1299_eeg_profile_t p;
    p.data_rate_code = dr_code;
    p.gain_code = ADS1299_GAIN_24;
    p.enable_srb1 = 0u;
    p.enable_srb2 = 0u;
    p.enable_bias = 1u;
    p.bias_use_internal_ref = 1u;
    p.bias_positive_mask = ADS1299_ALL_CHANNELS_MASK;
    p.bias_negative_mask = ADS1299_ALL_CHANNELS_MASK;
    p.enable_leadoff = 0u;
    p.leadoff_positive_mask = 0u;
    p.leadoff_negative_mask = 0u;
    return p;
}

ads1299_eeg_profile_t ads1299_profile_eeg_250sps(void) {
    return make_profile(ADS1299_DR_250SPS);
}

ads1299_eeg_profile_t ads1299_profile_eeg_500sps(void) {
    return make_profile(ADS1299_DR_500SPS);
}

ads1299_eeg_profile_t ads1299_profile_eeg_1ksps(void) {
    return make_profile(ADS1299_DR_1KSPS);
}

ads1299_status_t ads1299_profile_stop_streaming(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;

    ads1299_status_t st = ads1299_stop(dev);
    if (st != ADS1299_OK) return st;

    st = ads1299_sdatac(dev);
    if (st != ADS1299_OK) return st;

    return ADS1299_OK;
}

ads1299_status_t ads1299_profile_start_streaming(ads1299_t *dev) {
    if (!dev) return ADS1299_EINVAL;

    ads1299_status_t st = ads1299_rdatac(dev);
    if (st != ADS1299_OK) return st;

    st = ads1299_start(dev);
    if (st != ADS1299_OK) {
        (void)ads1299_sdatac(dev);
        return st;
    }
    return ADS1299_OK;
}

ads1299_status_t ads1299_profile_apply_eeg(ads1299_t *dev,
                                           const ads1299_eeg_profile_t *profile,
                                           int verify_registers) {
    if (!dev || !profile) return ADS1299_EINVAL;

    ads1299_status_t st = ads1299_profile_stop_streaming(dev);
    if (st != ADS1299_OK) return st;

    st = ads1299_set_data_rate(dev, profile->data_rate_code);
    if (st != ADS1299_OK) return st;

    const uint8_t mask = active_channel_mask(dev);
    const uint8_t count = (dev->channel_count == 4u || dev->channel_count == 6u || dev->channel_count == 8u)
                              ? dev->channel_count
                              : ADS1299_CHANNEL_COUNT;

    for (uint8_t ch = 1u; ch <= count; ++ch) {
        st = ads1299_set_channel(dev,
                                 ch,
                                 profile->gain_code,
                                 ADS1299_MUX_NORMAL,
                                 profile->enable_srb2 ? 1 : 0,
                                 0);
        if (st != ADS1299_OK) return st;
    }

    st = ads1299_set_srb1(dev, profile->enable_srb1 ? 1 : 0);
    if (st != ADS1299_OK) return st;

    ads1299_bias_config_t bias;
    bias.enable_reference_buffer = profile->enable_bias ? 1u : 0u;
    bias.use_internal_bias_reference = profile->bias_use_internal_ref ? 1u : 0u;
    bias.enable_bias_buffer = profile->enable_bias ? 1u : 0u;
    bias.enable_bias_measurement = 0u;
    bias.enable_bias_leadoff_sense = 0u;
    bias.positive_sense_mask = profile->enable_bias ? (uint8_t)(profile->bias_positive_mask & mask) : 0u;
    bias.negative_sense_mask = profile->enable_bias ? (uint8_t)(profile->bias_negative_mask & mask) : 0u;

    st = ads1299_configure_bias(dev, &bias);
    if (st != ADS1299_OK) return st;

    ads1299_leadoff_config_t loff;
    loff.comparator_threshold_code = ADS1299_LOFF_COMP_95_5;
    loff.current_code = ADS1299_LOFF_CURRENT_6NA;
    loff.frequency_code = ADS1299_LOFF_FREQ_7_8HZ;
    loff.positive_sense_mask = profile->enable_leadoff
                                   ? (uint8_t)(profile->leadoff_positive_mask & mask)
                                   : 0u;
    loff.negative_sense_mask = profile->enable_leadoff
                                   ? (uint8_t)(profile->leadoff_negative_mask & mask)
                                   : 0u;
    loff.current_flip_mask = 0u;
    loff.enable_comparators = profile->enable_leadoff ? 1u : 0u;

    st = ads1299_configure_lead_off(dev, &loff);
    if (st != ADS1299_OK) return st;

    if (verify_registers) {
        ads1299_configuration_t cfg;
        st = ads1299_read_configuration(dev, &cfg);
        if (st != ADS1299_OK) return st;

        uint8_t actual = 0u;
        st = ads1299_verify_register(dev,
                                     ADS1299_REG_CONFIG1,
                                     (uint8_t)(ADS1299_CONFIG1_RESERVED_BASE |
                                               (cfg.value[ADS1299_REG_CONFIG1] & ADS1299_CONFIG1_WRITABLE_MASK)),
                                     0xFFu,
                                     &actual);
        if (st != ADS1299_OK) return st;
    }

    return ADS1299_OK;
}

ads1299_status_t ads1299_profile_internal_test(ads1299_t *dev,
                                               uint8_t gain_code,
                                               uint8_t frequency_code,
                                               int amplitude_x2) {
    if (!dev) return ADS1299_EINVAL;

    ads1299_status_t st = ads1299_profile_stop_streaming(dev);
    if (st != ADS1299_OK) return st;

    return ads1299_configure_internal_test(dev, gain_code, amplitude_x2, frequency_code);
}

ads1299_status_t ads1299_profile_input_short(ads1299_t *dev,
                                             uint8_t gain_code) {
    if (!dev) return ADS1299_EINVAL;

    ads1299_status_t st = ads1299_profile_stop_streaming(dev);
    if (st != ADS1299_OK) return st;

    return ads1299_configure_input_short_test(dev, gain_code);
}
