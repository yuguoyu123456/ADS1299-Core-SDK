/* MCXA153 progressive bring-up: ADS1299 probe / ID validation.
 * Board-specific pins and SDK calls stay behind board_ads1299_hal(). */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_device_id_t id;

    if (board_ads1299_hal(&hal) != 0) return 10;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 11;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 12;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 20;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 21;
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK) return 30;
    if (!id.is_ads1299_family) return 31;

    /* Attach a debugger here: id contains the decoded ADS1299 identity. */
    return 0;
}
