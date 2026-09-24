/* Progressive CC1312R7 bring-up: bounded 250-SPS normal-input EEG capture.
 * Board/SimpleLink details stay behind the board binding and MCU port.
 * This example intentionally uses typed/shared ADS1299 helpers rather than
 * writing CONFIG1 or CHnSET register bytes in platform code. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "board_ads1299_binding.h"

static int clean_stop(ads1299_t *device)
{
    int rc = 0;
    if (ads1299_stop(device) != ADS1299_OK) rc = 1;
    if (ads1299_sdatac(device) != ADS1299_OK) rc = 1;
    return rc;
}

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_device_id_t identity;
    ads1299_frame_t frame;

    if (cc1312r7_ads1299_make_hal(&hal) != CC1312R7_ADS1299_BIND_OK) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 6;

    /* ADS1299_DR_250SPS is the named shared-core CONFIG1 data-rate code for
     * the nominal 250-SPS rate at the datasheet 2.048-MHz master clock. */
    if (ads1299_set_data_rate(&device, ADS1299_DR_250SPS) != ADS1299_OK)
        return 7;

    /* Normal electrode input, gain 24, SRB2 disabled. Configure only channels
     * physically present on the detected ADS1299-4/-6/-8 variant. */
    for (uint8_t channel = 1u; channel <= identity.channel_count; ++channel) {
        if (ads1299_set_channel(&device, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK)
            return 8;
    }

    if (ads1299_rdatac(&device) != ADS1299_OK) return 9;
    if (ads1299_start(&device) != ADS1299_OK) {
        (void)ads1299_sdatac(&device);
        return 10;
    }

    /* Capture one bounded second at the nominal 250-SPS profile. Keep network,
     * UART or file transport outside this DRDY-critical loop; the dedicated
     * streaming example can add a bounded queue without blocking acquisition. */
    for (unsigned sample = 0; sample < 250u; ++sample) {
        if (ads1299_wait_drdy(&device, 100000u, 100u) != ADS1299_OK) {
            (void)clean_stop(&device);
            return 11;
        }
        if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) {
            (void)clean_stop(&device);
            return 12;
        }
        /* Consume/copy frame.channel[] promptly; do not perform radio or
         * console blocking work in the DRDY timing path. */
    }

    if (clean_stop(&device) != 0) return 13;
    return 0;
}
