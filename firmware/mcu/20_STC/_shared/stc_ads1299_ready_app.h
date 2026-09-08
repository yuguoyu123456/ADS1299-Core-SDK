#ifndef STC_ADS1299_READY_APP_H
#define STC_ADS1299_READY_APP_H

#include <stdint.h>
#include "ads1299.h"
#include "ads1299_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STC_ADS1299_PROFILE_EEG_250 = 0,
    STC_ADS1299_PROFILE_EEG_500,
    STC_ADS1299_PROFILE_EEG_1000,
    STC_ADS1299_PROFILE_INTERNAL_TEST,
    STC_ADS1299_PROFILE_INPUT_SHORT
} stc_ads1299_profile_t;

typedef struct {
    uint32_t sample_index;
    uint32_t read_errors;
    uint32_t queue_overflows;
    ads1299_frame_t last_frame;
} stc_ads1299_state_t;

ads1299_status_t stc_ads1299_prepare(ads1299_t *dev,
                                     stc_ads1299_profile_t profile,
                                     int verify_registers);
ads1299_status_t stc_ads1299_start(ads1299_t *dev);
ads1299_status_t stc_ads1299_read_one(ads1299_t *dev,
                                      stc_ads1299_state_t *state);
ads1299_status_t stc_ads1299_stop(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif
