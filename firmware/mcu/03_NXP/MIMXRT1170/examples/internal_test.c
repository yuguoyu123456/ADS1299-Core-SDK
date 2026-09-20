/* MIMXRT1170 beginner step 2: ADS1299 internal test generator.
 * Hardware-dependent wiring remains in board/board_ads1299_config.h.
 * Shared ADS1299 core owns all register/profile behavior.
 */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

volatile ads1299_frame_t ads1299_internal_test_frame;
volatile int ads1299_internal_test_result;

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;

    ads1299_internal_test_result = 1;
    if (board_ads1299_hal(&hal) != 0) return 1;
    ads1299_internal_test_result = 2;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    ads1299_internal_test_result = 3;
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 3;
    ads1299_internal_test_result = 4;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 4;
    ads1299_internal_test_result = 5;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 5;
    ads1299_internal_test_result = 6;
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) return 6;

    ads1299_internal_test_result = 7;
    if (ads1299_configure_internal_test(&dev, ADS1299_GAIN_24, 0,
            ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return 7;
    ads1299_internal_test_result = 8;
    if (ads1299_rdatac(&dev) != ADS1299_OK) return 8;
    ads1299_internal_test_result = 9;
    if (ads1299_start(&dev) != ADS1299_OK) return 9;

    ads1299_internal_test_result = 0;
    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) {
                ads1299_internal_test_result = 10;
                (void)ads1299_stop(&dev);
                return 10;
            }
            ads1299_internal_test_frame = frame;
        }
    }
}
