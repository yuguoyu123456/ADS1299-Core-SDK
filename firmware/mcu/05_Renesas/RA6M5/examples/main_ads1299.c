/* Minimal integration example.  The board project supplies only the SDK
 * callbacks; all ADS1299 commands and register semantics remain in Core. */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Debugger snapshot only, not lossless transport. Odd sequence means writing. */
volatile ads1299_frame_t ads1299_latest_frame;
volatile uint32_t ads1299_frame_sequence;

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_frame_t frame;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 5;

    ads1299_device_id_t identity;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 6;
    if (ads1299_configure_internal_test(&device, ADS1299_GAIN_24, 0,
            ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return 7;
    if (ads1299_rdatac(&device) != ADS1299_OK) return 8;
    if (ads1299_start(&device) != ADS1299_OK) return 9;

    for (;;) {
        int ready = port.drdy_read(port.user);
        if (ready < 0) return 11;
        if (ready == 0) {
            if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK)
                return 10;
            ++ads1299_frame_sequence;
            ads1299_latest_frame = frame;
            ++ads1299_frame_sequence;
        }
    }
}
