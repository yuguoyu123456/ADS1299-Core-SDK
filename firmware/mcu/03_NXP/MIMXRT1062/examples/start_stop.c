/* MIMXRT1062 beginner step 5: clean continuous start/stop lifecycle.
 * Board wiring stays in board/ and ADS1299 protocol semantics stay in Core. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "board_ads1299_binding.h"

#define ADS1299_START_STOP_FRAMES 250u

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_port_t port;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    uint32_t frames = 0u;

    if (board_ads1299_make_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK ||
        !id.is_ads1299_family) return 6;

    if (ads1299_set_data_rate(&dev, ADS1299_DR_250SPS) != ADS1299_OK) return 7;
    if (ads1299_rdatac(&dev) != ADS1299_OK) return 8;
    if (ads1299_start(&dev) != ADS1299_OK) return 9;

    while (frames < ADS1299_START_STOP_FRAMES) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK)
                return 10;
            ++frames;
        }
    }

    /* Stop conversions first, then leave continuous-read mode so later
     * register operations begin from a deterministic command state. */
    if (ads1299_stop(&dev) != ADS1299_OK) return 11;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 12;

    return 0;
}
