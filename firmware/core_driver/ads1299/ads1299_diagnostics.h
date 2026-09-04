#ifndef ADS1299_DIAGNOSTICS_H
#define ADS1299_DIAGNOSTICS_H

#include <stdint.h>

#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TI SBAS499C Equation 3 temperature-sensor transfer constants. */
#define ADS1299_TEMP_SENSOR_25C_UV       145300.0
#define ADS1299_TEMP_SENSOR_UV_PER_C     490.0
#define ADS1299_TEMP_SENSOR_REFERENCE_C  25.0

/**
 * Convert the input-referred differential temperature-sensor reading, already
 * scaled to microvolts, to degrees Celsius using SBAS499C Equation 3.
 * Note: TI states that device self-heating can make this exceed PCB ambient.
 */
ads1299_status_t ads1299_temperature_c_from_microvolts(double sensor_uv,
                                                        double *temperature_c);

/**
 * Return the differential voltage presented to the ADC when MUX=MVDD.
 * SBAS499C: channels 1,2,5,6,7,8 -> 0.5*(AVDD+AVSS); channels 3,4 -> DVDD/4.
 * This is the internal MUX stimulus, not a generic reconstruction of a rail.
 */
ads1299_status_t ads1299_mvdd_mux_input_volts(uint8_t channel_1_to_8,
                                               double avdd_v,
                                               double avss_v,
                                               double dvdd_v,
                                               double *mux_input_v);

/** Configure one channel to the on-chip temperature sensor MUX. */
ads1299_status_t ads1299_configure_temperature_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8,
    uint8_t gain_code);

/**
 * Configure one channel to the on-chip MVDD supply-measurement MUX at gain=1,
 * matching TI's recommendation to avoid PGA saturation during supply checks.
 */
ads1299_status_t ads1299_configure_supply_measurement(
    ads1299_t *dev,
    uint8_t channel_1_to_8);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_DIAGNOSTICS_H */
