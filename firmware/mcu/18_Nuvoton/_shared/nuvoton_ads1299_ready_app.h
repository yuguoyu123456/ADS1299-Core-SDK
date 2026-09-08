#ifndef NUVOTON_ADS1299_READY_APP_H
#define NUVOTON_ADS1299_READY_APP_H

#include <stdint.h>
#include "ads1299.h"
#include "ads1299_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NUVOTON_ADS1299_PROFILE_EEG_250 = 0,
    NUVOTON_ADS1299_PROFILE_EEG_500,
    NUVOTON_ADS1299_PROFILE_EEG_1000,
    NUVOTON_ADS1299_PROFILE_INTERNAL_TEST,
    NUVOTON_ADS1299_PROFILE_INPUT_SHORT
} nuvoton_ads1299_profile_t;

typedef struct {
    uint32_t sample_index;
    uint32_t read_errors;
    uint32_t queue_overflows;
    ads1299_frame_t last_frame;
} nuvoton_ads1299_state_t;

ads1299_status_t nuvoton_ads1299_prepare(ads1299_t *dev,
                                         nuvoton_ads1299_profile_t profile,
                                         int verify_registers);
ads1299_status_t nuvoton_ads1299_start(ads1299_t *dev);
ads1299_status_t nuvoton_ads1299_read_one(ads1299_t *dev,
                                          nuvoton_ads1299_state_t *state);
ads1299_status_t nuvoton_ads1299_stop(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif
