#ifndef ADS1299_ESP32C3_ESPIDF_HAL_H
#define ADS1299_ESP32C3_ESPIDF_HAL_H

#include "ads1299_spi.h"
#include "driver/spi_master.h"

typedef struct {
    spi_device_handle_t spi;
} ads1299_espidf_hal_ctx_t;

int ads1299_espidf_hal_init(ads1299_espidf_hal_ctx_t *ctx,
                            ads1299_platform_hal_t *hal);

#endif
