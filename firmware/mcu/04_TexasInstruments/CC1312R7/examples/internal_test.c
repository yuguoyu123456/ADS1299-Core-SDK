/* Progressive CC1312R7 bring-up: ADS1299 internal test source.
 * Board/SimpleLink details stay behind the board binding and MCU port. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "board_ads1299_binding.h"

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

    /* Shared core owns CONFIG2/channel register semantics; the platform does
     * not duplicate ADS1299 register bytes. Gain=24, normal internal amplitude,
     * TI-defined fCLK/2^21 test frequency. */
    if (ads1299_configure_internal_test(&device, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK)
        return 7;
    if (ads1299_rdatac(&device) != ADS1299_OK) return 8;
    if (ads1299_start(&device) != ADS1299_OK) return 9;

    /* Capture a bounded validation window instead of trapping forever. */
    for (unsigned sample = 0; sample < 32u; ++sample) {
        unsigned wait = 0;
        while (port.drdy_read(port.user) != 0) {
            port.delay_us(port.user, 100u);
            if (++wait >= 10000u) {
                (void)ads1299_stop(&device);
                (void)ads1299_sdatac(&device);
                return 10; /* DRDY timeout */
            }
        }
        if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) {
            (void)ads1299_stop(&device);
            (void)ads1299_sdatac(&device);
            return 11;
        }
        /* Inspect/stream frame.channel[] in the application transport layer. */
    }

    if (ads1299_stop(&device) != ADS1299_OK) return 12;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 13;
    return 0;
}
