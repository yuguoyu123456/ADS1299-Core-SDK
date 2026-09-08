#ifndef ADS1299_PROFILES_H
#define ADS1299_PROFILES_H

#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t data_rate_code;       /* ADS1299_DR_* */
    uint8_t gain_code;            /* ADS1299_GAIN_* register-bit encoding */
    uint8_t enable_srb1;
    uint8_t enable_srb2;
    uint8_t enable_bias;
    uint8_t bias_use_internal_ref;
    uint8_t bias_positive_mask;
    uint8_t bias_negative_mask;
    uint8_t enable_leadoff;
    uint8_t leadoff_positive_mask;
    uint8_t leadoff_negative_mask;
} ads1299_eeg_profile_t;

/* Build a conservative 8-channel EEG profile: 250 SPS, gain 24, normal input,
 * BIAS enabled from all channels, no SRB and no lead-off injection. */
ads1299_eeg_profile_t ads1299_profile_eeg_250sps(void);
ads1299_eeg_profile_t ads1299_profile_eeg_500sps(void);
ads1299_eeg_profile_t ads1299_profile_eeg_1ksps(void);

/* Apply a profile while conversions are stopped. The function leaves the
 * device configured but not streaming; call ads1299_profile_start_streaming()
 * when the application is ready to consume DRDY events. */
ads1299_status_t ads1299_profile_apply_eeg(ads1299_t *dev,
                                           const ads1299_eeg_profile_t *profile,
                                           int verify_registers);

/* Configure the internal square-wave calibration signal on all active channels. */
ads1299_status_t ads1299_profile_internal_test(ads1299_t *dev,
                                               uint8_t gain_code,
                                               uint8_t frequency_code,
                                               int amplitude_x2);

/* Configure all active channels shorted to exercise ADC/PGA noise without electrodes. */
ads1299_status_t ads1299_profile_input_short(ads1299_t *dev,
                                             uint8_t gain_code);

/* Standard continuous-conversion start/stop sequence. */
ads1299_status_t ads1299_profile_start_streaming(ads1299_t *dev);
ads1299_status_t ads1299_profile_stop_streaming(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_PROFILES_H */
