#include "ads1299_diagnostics.h"

ads1299_status_t ads1299_temperature_c_from_microvolts(double sensor_uv,
                                                        double *temperature_c) {
    if (!temperature_c) return ADS1299_EINVAL;
    *temperature_c =
        ((sensor_uv - ADS1299_TEMP_SENSOR_25C_UV) /
         ADS1299_TEMP_SENSOR_UV_PER_C) + ADS1299_TEMP_SENSOR_REFERENCE_C;
    return ADS1299_OK;
}

ads1299_status_t ads1299_mvdd_mux_input_volts(uint8_t channel_1_to_8,
                                               double avdd_v,
                                               double avss_v,
                                               double dvdd_v,
                                               double *mux_input_v) {
    if (!mux_input_v || channel_1_to_8 < 1u || channel_1_to_8 > 8u) {
        return ADS1299_EINVAL;
    }

    if (channel_1_to_8 == 3u || channel_1_to_8 == 4u) {
        if (dvdd_v <= 0.0) return ADS1299_EINVAL;
        *mux_input_v = dvdd_v / 4.0;
        return ADS1299_OK;
    }

    if (avdd_v <= avss_v) return ADS1299_EINVAL;
    *mux_input_v = 0.5 * (avdd_v + avss_v);
    return ADS1299_OK;
}

ads1299_status_t ads1299_configure_temperature_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8,
    uint8_t gain_code) {
    return ads1299_set_channel(dev, channel_1_to_8, gain_code,
                               ADS1299_MUX_TEMP, 0, 0);
}

ads1299_status_t ads1299_configure_supply_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8) {
    return ads1299_set_channel(dev, channel_1_to_8, ADS1299_GAIN_1,
                               ADS1299_MUX_MVDD, 0, 0);
}
