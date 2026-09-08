#include "rp_ads1299_ready_app.h"
#include "ads1299_profiles.h"
#include "ads1299_regs.h"

ads1299_status_t rp_ads1299_prepare(ads1299_t *dev,
                                   rp_ads1299_profile_t profile,
                                   int verify_registers)
{
    ads1299_status_t st;
    ads1299_eeg_profile_t eeg;
    if (!dev) return ADS1299_EINVAL;
    st = ads1299_sdatac(dev);
    if (st != ADS1299_OK) return st;

    switch (profile) {
    case RP_ADS1299_PROFILE_EEG_250:
        eeg = ads1299_profile_eeg_250sps();
        return ads1299_profile_apply_eeg(dev, &eeg, verify_registers);
    case RP_ADS1299_PROFILE_EEG_500:
        eeg = ads1299_profile_eeg_500sps();
        return ads1299_profile_apply_eeg(dev, &eeg, verify_registers);
    case RP_ADS1299_PROFILE_EEG_1000:
        eeg = ads1299_profile_eeg_1ksps();
        return ads1299_profile_apply_eeg(dev, &eeg, verify_registers);
    case RP_ADS1299_PROFILE_INTERNAL_TEST:
        return ads1299_profile_internal_test(dev, ADS1299_GAIN_24,
                                             ADS1299_TEST_FREQ_FCLK_2_21, 0);
    case RP_ADS1299_PROFILE_INPUT_SHORT:
        return ads1299_profile_input_short(dev, ADS1299_GAIN_24);
    default:
        return ADS1299_EINVAL;
    }
}

ads1299_status_t rp_ads1299_start(ads1299_t *dev)
{
    return dev ? ads1299_profile_start_streaming(dev) : ADS1299_EINVAL;
}

ads1299_status_t rp_ads1299_read_one(ads1299_t *dev,
                                    rp_ads1299_ready_state_t *state)
{
    ads1299_status_t st;
    if (!dev || !state) return ADS1299_EINVAL;
    st = ads1299_read_frame_continuous(dev, &state->last_frame);
    if (st == ADS1299_OK) state->sample_index++;
    else state->read_errors++;
    return st;
}

ads1299_status_t rp_ads1299_stop(ads1299_t *dev)
{
    return dev ? ads1299_profile_stop_streaming(dev) : ADS1299_EINVAL;
}
