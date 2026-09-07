#ifndef ADS1299_PORT_PICO_H
#define ADS1299_PORT_PICO_H

#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "ads1299_port.h"

typedef struct {
    spi_inst_t *spi;
    uint cs_pin;
    int reset_pin;
    int start_pin;
    int drdy_pin;
} ads1299_pico_ctx_t;

ads1299_port_t ads1299_pico_make_port(ads1299_pico_ctx_t *ctx);

#endif
