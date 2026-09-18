/* MIMXRT1042 progressive bring-up: ADS1299 input-short validation. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;

    if (board_ads1299_hal(&hal) != 0) return 10;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 11;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 12;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 20;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 21;
    if (ads1299_configure_input_short(&dev, ADS1299_GAIN_24) != ADS1299_OK)
        return 40;
    if (ads1299_rdatac(&dev) != ADS1299_OK) return 41;
    if (ads1299_start(&dev) != ADS1299_OK) return 42;

    for (;;) {
        int ready = port.drdy_read(port.user);
        if (ready < 0) return 51;
        if (ready == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK)
                return 50;
            /* Shorted-input samples should remain near the converter baseline. */
        }
    }
}
