/* CC1312R7 progressive example: initialize the board binding and validate ADS1299 ID.
 * Hardware-dependent SPI/GPIO assignments stay in board_ads1299_config.h. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "board_ads1299_binding.h"

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_device_id_t identity;
    cc1312r7_ads1299_board_config_t cfg = cc1312r7_ads1299_board_config_default();

    if (cc1312r7_ads1299_make_hal(&cfg, &hal) != CC1312R7_ADS1299_BIND_OK) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK) return 6;
    if (!identity.is_ads1299_family) return 7;

    /* Attach a debugger here: identity now contains the decoded device ID. */
    return 0;
}
