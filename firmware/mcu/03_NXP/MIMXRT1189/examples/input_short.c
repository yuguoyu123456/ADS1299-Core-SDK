/* MIMXRT1189 beginner step 3: input-short validation using the shared core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Inspect these from the debugger when a console is not yet available. */
volatile int ads1299_input_short_result = -1;
volatile ads1299_frame_t ads1299_input_short_frame;

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;

    if (board_ads1299_hal(&hal) != 0) {
        ads1299_input_short_result = 1;
        return 1;
    }
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) {
        ads1299_input_short_result = 2;
        return 2;
    }

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) {
        ads1299_input_short_result = 3;
        return 3;
    }
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) {
        ads1299_input_short_result = 4;
        return 4;
    }
    if (ads1299_sdatac(&dev) != ADS1299_OK) {
        ads1299_input_short_result = 5;
        return 5;
    }
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) {
        ads1299_input_short_result = 6;
        return 6;
    }
    if (ads1299_configure_input_short_test(&dev, ADS1299_GAIN_24) != ADS1299_OK) {
        ads1299_input_short_result = 7;
        return 7;
    }
    if (ads1299_rdatac(&dev) != ADS1299_OK) {
        ads1299_input_short_result = 8;
        return 8;
    }
    if (ads1299_start(&dev) != ADS1299_OK) {
        ads1299_input_short_result = 9;
        return 9;
    }

    ads1299_input_short_result = 0;
    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) {
                ads1299_input_short_result = 10;
                return 10;
            }
            ads1299_input_short_frame = frame;
        }
    }
}
