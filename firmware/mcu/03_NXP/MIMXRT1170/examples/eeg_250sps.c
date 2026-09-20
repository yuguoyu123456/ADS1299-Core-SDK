/* MIMXRT1170 beginner step 4: 250-SPS normal-electrode acquisition.
 * ADS1299 register semantics remain in the shared core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Inspect these from the debugger when a console is not yet available.
 * sequence is odd while latest_frame is being replaced and even when stable. */
volatile int ads1299_eeg250_result = -1;
volatile ads1299_frame_t ads1299_eeg250_latest_frame;
volatile uint32_t ads1299_eeg250_sequence;

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    uint8_t channel;

    if (board_ads1299_hal(&hal) != 0) {
        ads1299_eeg250_result = 1;
        return 1;
    }
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) {
        ads1299_eeg250_result = 2;
        return 2;
    }

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) {
        ads1299_eeg250_result = 3;
        return 3;
    }
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) {
        ads1299_eeg250_result = 4;
        return 4;
    }
    if (ads1299_sdatac(&dev) != ADS1299_OK) {
        ads1299_eeg250_result = 5;
        return 5;
    }
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) {
        ads1299_eeg250_result = 6;
        return 6;
    }

    /* TI ADS1299 CONFIG1 DR=110: nominal 250 SPS at fCLK=2.048 MHz. */
    if (ads1299_set_data_rate(&dev, ADS1299_DR_250SPS) != ADS1299_OK) {
        ads1299_eeg250_result = 7;
        return 7;
    }
    for (channel = 1u; channel <= id.channel_count; ++channel) {
        if (ads1299_set_channel(&dev, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) {
            ads1299_eeg250_result = 8;
            return 8;
        }
    }
    if (ads1299_rdatac(&dev) != ADS1299_OK) {
        ads1299_eeg250_result = 9;
        return 9;
    }
    if (ads1299_start(&dev) != ADS1299_OK) {
        ads1299_eeg250_result = 10;
        return 10;
    }

    ads1299_eeg250_result = 0;
    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) {
                ads1299_eeg250_result = 11;
                return 11;
            }
            ++ads1299_eeg250_sequence;
            ads1299_eeg250_latest_frame = frame;
            ++ads1299_eeg250_sequence;
        }
    }
}
