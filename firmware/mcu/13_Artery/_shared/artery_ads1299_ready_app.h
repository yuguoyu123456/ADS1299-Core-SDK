#ifndef ARTERY_ADS1299_READY_APP_H
#define ARTERY_ADS1299_READY_APP_H

#include <stdint.h>
#include "ads1299.h"
#include "ads1299_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ARTERY_ADS1299_PROFILE_EEG_250 = 0,
    ARTERY_ADS1299_PROFILE_EEG_500,
    ARTERY_ADS1299_PROFILE_EEG_1000,
    ARTERY_ADS1299_PROFILE_INTERNAL_TEST,
    ARTERY_ADS1299_PROFILE_INPUT_SHORT
} artery_ads1299_profile_t;

typedef struct {
    uint32_t sample_index;
    uint32_t read_errors;
    uint32_t queue_overflows;
    ads1299_frame_t last_frame;
} artery_ads1299_state_t;

ads1299_status_t artery_ads1299_prepare(ads1299_t *dev,
                                        artery_ads1299_profile_t profile,
                                        int verify_registers);
ads1299_status_t artery_ads1299_start(ads1299_t *dev);
ads1299_status_t artery_ads1299_read_one(ads1299_t *dev,
                                         artery_ads1299_state_t *state);
ads1299_status_t artery_ads1299_stop(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* ARTERY_ADS1299_READY_APP_H */
