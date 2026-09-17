/* MCXA156 beginner step 4: 250-SPS normal-electrode acquisition.
 * ADS1299 register semantics remain in the shared core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

volatile ads1299_frame_t ads1299_eeg250_latest_frame;
volatile uint32_t ads1299_eeg250_sequence;

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    uint8_t channel;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) return 6;

    /* ADS1299 CONFIG1 DR=110: nominal 250 SPS at fCLK=2.048 MHz. */
    if (ads1299_set_data_rate(&dev, ADS1299_DR_250SPS) != ADS1299_OK) return 7;
    for (channel = 1u; channel <= id.channel_count; ++channel) {
        if (ads1299_set_channel(&dev, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) return 8;
    }
    if (ads1299_rdatac(&dev) != ADS1299_OK) return 9;
    if (ads1299_start(&dev) != ADS1299_OK) return 10;

    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) return 11;
            ++ads1299_eeg250_sequence;
            ads1299_eeg250_latest_frame = frame;
            ++ads1299_eeg250_sequence;
        }
    }
}
