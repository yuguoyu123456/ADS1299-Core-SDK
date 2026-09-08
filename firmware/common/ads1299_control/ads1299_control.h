#ifndef ADS1299_CONTROL_H
#define ADS1299_CONTROL_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ads1299_control.h
 * @brief Optional high-level control layer built on the portable ADS1299 core.
 *
 * This module does not duplicate register ownership from core_driver/ads1299.
 * It provides ergonomic operations for interactive bring-up, examples and
 * laboratory tooling while delegating all device semantics to the core API.
 */

typedef struct {
    ads1299_t *dev;
} ads1299_control_t;

typedef enum {
    ADS1299_CONTROL_INPUT_NORMAL = ADS1299_MUX_NORMAL,
    ADS1299_CONTROL_INPUT_SHORTED = ADS1299_MUX_SHORTED,
    ADS1299_CONTROL_INPUT_BIAS_MEAS = ADS1299_MUX_BIAS_MEAS,
    ADS1299_CONTROL_INPUT_MVDD = ADS1299_MUX_MVDD,
    ADS1299_CONTROL_INPUT_TEMP = ADS1299_MUX_TEMP,
    ADS1299_CONTROL_INPUT_TEST = ADS1299_MUX_TEST,
    ADS1299_CONTROL_INPUT_BIAS_DRP = ADS1299_MUX_BIAS_DRP,
    ADS1299_CONTROL_INPUT_BIAS_DRN = ADS1299_MUX_BIAS_DRN
} ads1299_control_input_t;

typedef enum {
    ADS1299_CONTROL_PROFILE_EEG = 0,
    ADS1299_CONTROL_PROFILE_INPUT_SHORT,
    ADS1299_CONTROL_PROFILE_INTERNAL_TEST
} ads1299_control_profile_t;

typedef struct {
    uint32_t sample_rate_sps;
    unsigned gain;
    int enable_srb1;
    int enable_bias;
    int use_internal_bias_reference;
    uint8_t positive_bias_mask;
    uint8_t negative_bias_mask;
} ads1299_control_profile_options_t;

ads1299_status_t ads1299_control_init(ads1299_control_t *control,
                                      ads1299_t *dev);

ads1299_status_t ads1299_control_set_rate_sps(ads1299_control_t *control,
                                              uint32_t sample_rate_sps);
ads1299_status_t ads1299_control_set_gain(ads1299_control_t *control,
                                         uint8_t channel_1_to_8,
                                         unsigned gain);
ads1299_status_t ads1299_control_set_input(ads1299_control_t *control,
                                          uint8_t channel_1_to_8,
                                          ads1299_control_input_t input);
ads1299_status_t ads1299_control_set_channel_enabled(
    ads1299_control_t *control,
    uint8_t channel_1_to_8,
    int enable);
ads1299_status_t ads1299_control_set_srb1(ads1299_control_t *control,
                                         int enable);
ads1299_status_t ads1299_control_set_srb2(ads1299_control_t *control,
                                         uint8_t channel_1_to_8,
                                         int enable);

ads1299_status_t ads1299_control_read_bias_config(
    ads1299_control_t *control,
    ads1299_bias_config_t *config);
ads1299_status_t ads1299_control_set_bias_enabled(ads1299_control_t *control,
                                                 int enable);
ads1299_status_t ads1299_control_set_bias_reference_internal(
    ads1299_control_t *control,
    int internal_reference);
ads1299_status_t ads1299_control_set_bias_masks(ads1299_control_t *control,
                                               uint8_t positive_mask,
                                               uint8_t negative_mask);

ads1299_status_t ads1299_control_configure_test(
    ads1299_control_t *control,
    int amplitude_x2,
    uint8_t frequency_code);

void ads1299_control_profile_defaults(
    ads1299_control_profile_t profile,
    ads1299_control_profile_options_t *options);
ads1299_status_t ads1299_control_apply_profile(
    ads1299_control_t *control,
    ads1299_control_profile_t profile,
    const ads1299_control_profile_options_t *options);

ads1299_status_t ads1299_control_start_stream(ads1299_control_t *control);
ads1299_status_t ads1299_control_stop_stream(ads1299_control_t *control);

/**
 * Capture exactly frame_capacity frames into caller-owned storage.
 *
 * No large static buffer is allocated by this module. Each conversion waits for
 * active-low DRDY through ads1299_wait_drdy(), so the platform port must provide
 * drdy_read. On any failure the function attempts STOP + SDATAC before return.
 */
ads1299_status_t ads1299_control_capture_frames(
    ads1299_control_t *control,
    ads1299_frame_t *frames,
    size_t frame_capacity,
    uint32_t drdy_timeout_us,
    uint32_t poll_interval_us,
    size_t *captured);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_CONTROL_H */
