#ifndef ADS1299_PORT_ESP_IDF_H
#define ADS1299_PORT_ESP_IDF_H

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "ads1299_port.h"

typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    gpio_num_t reset_pin;
    gpio_num_t start_pin;
    gpio_num_t drdy_pin;
} ads1299_esp_idf_ctx_t;

ads1299_port_t ads1299_esp_idf_make_port(ads1299_esp_idf_ctx_t *ctx);

#endif
