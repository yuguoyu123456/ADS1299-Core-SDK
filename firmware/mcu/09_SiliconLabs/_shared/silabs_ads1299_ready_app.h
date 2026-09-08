#ifndef SILABS_ADS1299_READY_APP_H
#define SILABS_ADS1299_READY_APP_H

#include <stdint.h>
#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SILABS_ADS1299_PROFILE_EEG_250 = 0,
    SILABS_ADS1299_PROFILE_EEG_500,
    SILABS_ADS1299_PROFILE_EEG_1000,
    SILABS_ADS1299_PROFILE_INTERNAL_TEST,
    SILABS_ADS1299_PROFILE_INPUT_SHORT
} silabs_ads1299_profile_t;

typedef struct {
    uint32_t sample_index;
    uint32_t read_errors;
    uint32_t queue_overflows;
    ads1299_frame_t last_frame;
} silabs_ads1299_ready_state_t;

ads1299_status_t silabs_ads1299_prepare(ads1299_t *dev,
                                       silabs_ads1299_profile_t profile,
                                       int verify_registers);
ads1299_status_t silabs_ads1299_start(ads1299_t *dev);
ads1299_status_t silabs_ads1299_read_one(ads1299_t *dev,
                                        silabs_ads1299_ready_state_t *state);
ads1299_status_t silabs_ads1299_stop(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif
