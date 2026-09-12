#ifndef ADS1299_ESP32C2_ESPIDF_HAL_H
#define ADS1299_ESP32C2_ESPIDF_HAL_H

#include "ads1299_spi.h"
#include "driver/spi_master.h"

typedef struct {
    spi_device_handle_t spi;
} ads1299_espidf_hal_ctx_t;

/*
 * Initialize the ESP-IDF SPI/GPIO backend and expose it through the existing
 * platform callback contract.  Hardware-dependent pins live only in
 * board/esp8684_devkitm1_ads1299.h.
 */
int ads1299_espidf_hal_init(ads1299_espidf_hal_ctx_t *ctx,
                            ads1299_platform_hal_t *hal);

#endif
