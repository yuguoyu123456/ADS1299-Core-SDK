#ifndef ADS1299_ESP_IDF_HAL_H
#define ADS1299_ESP_IDF_HAL_H

#include "ads1299_spi.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    gpio_num_t reset_pin;
    gpio_num_t pwdn_pin;
    gpio_num_t start_pin;
    gpio_num_t drdy_pin;
} ads1299_esp_idf_ctx_t;

int ads1299_esp_idf_make_hal(ads1299_platform_hal_t *hal,
                             ads1299_esp_idf_ctx_t *ctx);

#endif
