#include "ads1299_control.h"

#include <string.h>

static ads1299_status_t check_control(const ads1299_control_t *control) {
    if (!control || !control->dev) return ADS1299_EINVAL;
    return ADS1299_OK;
}

static uint8_t active_channel_count(const ads1299_control_t *control) {
    if (control && control->dev &&
        (control->dev->channel_count == 4u ||
         control->dev->channel_count == 6u ||
         control->dev->channel_count == 8u)) {
        return control->dev->channel_count;
    }
    return ADS1299_CHANNEL_COUNT;
}

static int valid_channel(const ads1299_control_t *control, uint8_t channel) {
    return channel >= 1u && channel <= active_channel_count(control);
}

static ads1299_status_t gain_to_code(unsigned gain, uint8_t *code) {
    if (!code) return ADS1299_EINVAL;
    switch (gain) {
        case 1u:  *code = ADS1299_GAIN_1; break;
        case 2u:  *code = ADS1299_GAIN_2; break;
        case 4u:  *code = ADS1299_GAIN_4; break;
        case 6u:  *code = ADS1299_GAIN_6; break;
        case 8u:  *code = ADS1299_GAIN_8; break;
        case 12u: *code = ADS1299_GAIN_12; break;
        case 24u: *code = ADS1299_GAIN_24; break;
        default: return ADS1299_EINVAL;
    }
    return ADS1299_OK;
}

static ads1299_status_t rate_to_code(uint32_t sps, uint8_t *code) {
    if (!code) return ADS1299_EINVAL;
    switch (sps) {
        case 16000u: *code = ADS1299_DR_16KSPS; break;
        case 8000u:  *code = ADS1299_DR_8KSPS; break;
        case 4000u:  *code = ADS1299_DR_4KSPS; break;
        case 2000u:  *code = ADS1299_DR_2KSPS; break;
        case 1000u:  *code = ADS1299_DR_1KSPS; break;
        case 500u:   *code = ADS1299_DR_500SPS; break;
        case 250u:   *code = ADS1299_DR_250SPS; break;
        default: return ADS1299_EINVAL;
    }
    return ADS1299_OK;
}

static ads1299_status_t read_channel_register(ads1299_control_t *control,
                                              uint8_t channel,
                                              uint8_t *value) {
    if (check_control(control) != ADS1299_OK || !value ||
        !valid_channel(control, channel)) {
        return ADS1299_EINVAL;
    }
    return ads1299_read_register(control->dev,
                                 (uint8_t)(ADS1299_REG_CH1SET + channel - 1u),
                                 value);
}

static ads1299_status_t write_channel_preserving(
    ads1299_control_t *control,
    uint8_t channel,
    uint8_t value,
    int replace_gain,
    uint8_t gain_code,
    int replace_mux,
    uint8_t mux_code,
    int replace_power,
    int power_down) {
    uint8_t gain = (uint8_t)(value & ADS1299_CH_GAIN_MASK);
    uint8_t mux = (uint8_t)(value & ADS1299_CH_MUX_MASK);
    int srb2 = (value & ADS1299_CH_SRB2) != 0u;
    int pd = (value & ADS1299_CH_POWER_DOWN) != 0u;

    if (replace_gain) gain = gain_code;
    if (replace_mux) mux = mux_code;
    if (replace_power) pd = power_down ? 1 : 0;

    return ads1299_set_channel(control->dev, channel, gain, mux, srb2, pd);
}

ads1299_status_t ads1299_control_init(ads1299_control_t *control,
                                      ads1299_t *dev) {
    if (!control || !dev) return ADS1299_EINVAL;
    control->dev = dev;
    return ADS1299_OK;
}

ads1299_status_t ads1299_control_set_rate_sps(ads1299_control_t *control,
                                              uint32_t sample_rate_sps) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;
    uint8_t code = 0u;
    ads1299_status_t rc = rate_to_code(sample_rate_sps, &code);
    if (rc != ADS1299_OK) return rc;
    return ads1299_set_data_rate(control->dev, code);
}

ads1299_status_t ads1299_control_set_gain(ads1299_control_t *control,
                                         uint8_t channel_1_to_8,
                                         unsigned gain) {
    uint8_t gain_code = 0u;
    uint8_t current = 0u;
    ads1299_status_t rc = gain_to_code(gain, &gain_code);
    if (rc != ADS1299_OK) return rc;
    rc = read_channel_register(control, channel_1_to_8, &current);
    if (rc != ADS1299_OK) return rc;
    return write_channel_preserving(control, channel_1_to_8, current,
                                    1, gain_code, 0, 0u, 0, 0);
}

ads1299_status_t ads1299_control_set_input(ads1299_control_t *control,
                                          uint8_t channel_1_to_8,
                                          ads1299_control_input_t input) {
    if ((unsigned)input > ADS1299_MUX_BIAS_DRN) return ADS1299_EINVAL;
    uint8_t current = 0u;
    ads1299_status_t rc = read_channel_register(control, channel_1_to_8,
                                                &current);
    if (rc != ADS1299_OK) return rc;
    return write_channel_preserving(control, channel_1_to_8, current,
                                    0, 0u, 1, (uint8_t)input, 0, 0);
}

ads1299_status_t ads1299_control_set_channel_enabled(
    ads1299_control_t *control,
    uint8_t channel_1_to_8,
    int enable) {
    uint8_t current = 0u;
    ads1299_status_t rc = read_channel_register(control, channel_1_to_8,
                                                &current);
    if (rc != ADS1299_OK) return rc;
    return write_channel_preserving(control, channel_1_to_8, current,
                                    0, 0u, 0, 0u, 1, enable ? 0 : 1);
}

ads1299_status_t ads1299_control_set_srb1(ads1299_control_t *control,
                                         int enable) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;
    return ads1299_set_srb1(control->dev, enable ? 1 : 0);
}

ads1299_status_t ads1299_control_set_srb2(ads1299_control_t *control,
                                         uint8_t channel_1_to_8,
                                         int enable) {
    if (check_control(control) != ADS1299_OK ||
        !valid_channel(control, channel_1_to_8)) {
        return ADS1299_EINVAL;
    }
    return ads1299_set_channel_srb2(control->dev, channel_1_to_8,
                                    enable ? 1 : 0);
}

ads1299_status_t ads1299_control_read_bias_config(
    ads1299_control_t *control,
    ads1299_bias_config_t *config) {
    if (check_control(control) != ADS1299_OK || !config) {
        return ADS1299_EINVAL;
    }

    uint8_t config3 = 0u;
    uint8_t positive = 0u;
    uint8_t negative = 0u;
    ads1299_status_t rc = ads1299_read_register(control->dev,
                                                ADS1299_REG_CONFIG3,
                                                &config3);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_read_register(control->dev, ADS1299_REG_BIAS_SENSP,
                               &positive);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_read_register(control->dev, ADS1299_REG_BIAS_SENSN,
                               &negative);
    if (rc != ADS1299_OK) return rc;

    memset(config, 0, sizeof(*config));
    config->enable_reference_buffer =
        (config3 & ADS1299_CONFIG3_PD_REFBUF) ? 1u : 0u;
    config->use_internal_bias_reference =
        (config3 & ADS1299_CONFIG3_BIASREF_INT) ? 1u : 0u;
    config->enable_bias_buffer =
        (config3 & ADS1299_CONFIG3_PD_BIAS) ? 1u : 0u;
    config->enable_bias_measurement =
        (config3 & ADS1299_CONFIG3_BIAS_MEAS) ? 1u : 0u;
    config->enable_bias_leadoff_sense =
        (config3 & ADS1299_CONFIG3_BIAS_LOFF_SENS) ? 1u : 0u;
    config->positive_sense_mask = positive;
    config->negative_sense_mask = negative;
    return ADS1299_OK;
}

ads1299_status_t ads1299_control_set_bias_enabled(ads1299_control_t *control,
                                                 int enable) {
    ads1299_bias_config_t config;
    ads1299_status_t rc = ads1299_control_read_bias_config(control, &config);
    if (rc != ADS1299_OK) return rc;
    config.enable_bias_buffer = enable ? 1u : 0u;
    return ads1299_configure_bias(control->dev, &config);
}

ads1299_status_t ads1299_control_set_bias_reference_internal(
    ads1299_control_t *control,
    int internal_reference) {
    ads1299_bias_config_t config;
    ads1299_status_t rc = ads1299_control_read_bias_config(control, &config);
    if (rc != ADS1299_OK) return rc;
    config.use_internal_bias_reference = internal_reference ? 1u : 0u;
    return ads1299_configure_bias(control->dev, &config);
}

ads1299_status_t ads1299_control_set_bias_masks(ads1299_control_t *control,
                                               uint8_t positive_mask,
                                               uint8_t negative_mask) {
    ads1299_bias_config_t config;
    ads1299_status_t rc = ads1299_control_read_bias_config(control, &config);
    if (rc != ADS1299_OK) return rc;
    config.positive_sense_mask = positive_mask;
    config.negative_sense_mask = negative_mask;
    return ads1299_configure_bias(control->dev, &config);
}

ads1299_status_t ads1299_control_configure_test(
    ads1299_control_t *control,
    int amplitude_x2,
    uint8_t frequency_code) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;
    const ads1299_test_signal_config_t config = {
        .use_internal_source = 1u,
        .amplitude_x2 = amplitude_x2 ? 1u : 0u,
        .frequency_code = frequency_code,
    };
    return ads1299_configure_test_signal(control->dev, &config);
}

void ads1299_control_profile_defaults(
    ads1299_control_profile_t profile,
    ads1299_control_profile_options_t *options) {
    if (!options) return;
    memset(options, 0, sizeof(*options));
    options->sample_rate_sps = 250u;
    options->gain = 24u;
    options->use_internal_bias_reference = 1;

    if (profile == ADS1299_CONTROL_PROFILE_EEG) {
        /* Neutral core-board default: no assumed reference or BIAS electrode
         * topology. Applications explicitly opt in below. */
        options->enable_srb1 = 0;
        options->enable_bias = 0;
    }
}

ads1299_status_t ads1299_control_apply_profile(
    ads1299_control_t *control,
    ads1299_control_profile_t profile,
    const ads1299_control_profile_options_t *options) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;

    ads1299_control_profile_options_t local;
    if (!options) {
        ads1299_control_profile_defaults(profile, &local);
        options = &local;
    }

    uint8_t gain_code = 0u;
    ads1299_status_t rc = gain_to_code(options->gain, &gain_code);
    if (rc != ADS1299_OK) return rc;

    rc = ads1299_control_set_rate_sps(control, options->sample_rate_sps);
    if (rc != ADS1299_OK) return rc;

    switch (profile) {
        case ADS1299_CONTROL_PROFILE_EEG: {
            const uint8_t count = active_channel_count(control);
            for (uint8_t ch = 1u; ch <= count; ++ch) {
                rc = ads1299_set_channel(control->dev, ch, gain_code,
                                         ADS1299_MUX_NORMAL, 0, 0);
                if (rc != ADS1299_OK) return rc;
            }
            rc = ads1299_set_srb1(control->dev,
                                  options->enable_srb1 ? 1 : 0);
            if (rc != ADS1299_OK) return rc;
            break;
        }

        case ADS1299_CONTROL_PROFILE_INPUT_SHORT:
            rc = ads1299_configure_input_short_test(control->dev, gain_code);
            if (rc != ADS1299_OK) return rc;
            rc = ads1299_set_srb1(control->dev, 0);
            if (rc != ADS1299_OK) return rc;
            break;

        case ADS1299_CONTROL_PROFILE_INTERNAL_TEST:
            rc = ads1299_configure_internal_test(
                control->dev, gain_code, 0,
                ADS1299_TEST_FREQ_FCLK_2_21);
            if (rc != ADS1299_OK) return rc;
            rc = ads1299_set_srb1(control->dev, 0);
            if (rc != ADS1299_OK) return rc;
            break;

        default:
            return ADS1299_EINVAL;
    }

    ads1299_bias_config_t bias;
    memset(&bias, 0, sizeof(bias));
    bias.enable_reference_buffer = 1u;
    bias.use_internal_bias_reference =
        options->use_internal_bias_reference ? 1u : 0u;

    if (profile == ADS1299_CONTROL_PROFILE_EEG && options->enable_bias) {
        bias.enable_bias_buffer = 1u;
        bias.positive_sense_mask = options->positive_bias_mask;
        bias.negative_sense_mask = options->negative_bias_mask;
    }

    return ads1299_configure_bias(control->dev, &bias);
}

ads1299_status_t ads1299_control_start_stream(ads1299_control_t *control) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;
    ads1299_status_t rc = ads1299_rdatac(control->dev);
    if (rc != ADS1299_OK) return rc;
    rc = ads1299_start(control->dev);
    if (rc != ADS1299_OK) {
        (void)ads1299_sdatac(control->dev);
    }
    return rc;
}

ads1299_status_t ads1299_control_stop_stream(ads1299_control_t *control) {
    if (check_control(control) != ADS1299_OK) return ADS1299_EINVAL;
    ads1299_status_t first = ads1299_stop(control->dev);
    ads1299_status_t second = ads1299_sdatac(control->dev);
    return first != ADS1299_OK ? first : second;
}

ads1299_status_t ads1299_control_capture_frames(
    ads1299_control_t *control,
    ads1299_frame_t *frames,
    size_t frame_capacity,
    uint32_t drdy_timeout_us,
    uint32_t poll_interval_us,
    size_t *captured) {
    if (captured) *captured = 0u;
    if (check_control(control) != ADS1299_OK || !frames ||
        frame_capacity == 0u) {
        return ADS1299_EINVAL;
    }

    ads1299_status_t rc = ads1299_control_start_stream(control);
    if (rc != ADS1299_OK) return rc;

    size_t count = 0u;
    for (; count < frame_capacity; ++count) {
        rc = ads1299_wait_drdy(control->dev, drdy_timeout_us,
                               poll_interval_us);
        if (rc != ADS1299_OK) break;
        rc = ads1299_read_frame_continuous(control->dev, &frames[count]);
        if (rc != ADS1299_OK) break;
        if (captured) *captured = count + 1u;
    }

    ads1299_status_t stop_rc = ads1299_control_stop_stream(control);
    if (rc != ADS1299_OK) return rc;
    return stop_rc;
}
