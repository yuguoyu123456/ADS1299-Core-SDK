#ifndef STM32_ADS1299_READY_APP_H
#define STM32_ADS1299_READY_APP_H

#include <stdint.h>
#include "ads1299.h"
#include "ads1299_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STM32_ADS1299_PROFILE_EEG_250 = 0,
    STM32_ADS1299_PROFILE_EEG_500,
    STM32_ADS1299_PROFILE_EEG_1000,
    STM32_ADS1299_PROFILE_INTERNAL_TEST,
    STM32_ADS1299_PROFILE_INPUT_SHORT
} stm32_ads1299_profile_t;

typedef struct {
    uint32_t sample_index;
    uint32_t dropped_or_late_frames;
    ads1299_frame_t last_frame;
} stm32_ads1299_stream_state_t;

ads1299_status_t stm32_ads1299_prepare(ads1299_t *dev,
                                       stm32_ads1299_profile_t profile);
ads1299_status_t stm32_ads1299_start(ads1299_t *dev,
                                     stm32_ads1299_stream_state_t *state);
ads1299_status_t stm32_ads1299_read_one(ads1299_t *dev,
                                        stm32_ads1299_stream_state_t *state);
ads1299_status_t stm32_ads1299_stop(ads1299_t *dev);

#ifdef __cplusplus
}
#endif

#endif
