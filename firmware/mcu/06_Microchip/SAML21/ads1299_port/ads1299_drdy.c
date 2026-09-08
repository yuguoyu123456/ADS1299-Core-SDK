#include "ads1299_drdy.h"
#include "ads1299_spi.h"

int ads1299_port_drdy_read(void *user) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    return ads1299_mcu_data_ready(ctx) ? 0 : 1;
}

int ads1299_mcu_data_ready(ads1299_mcu_port_t *ctx) {
    if (!ctx || !ctx->hal.pin_read) return 0;
    return ctx->hal.pin_read(ctx->hal.user, ADS1299_PIN_DRDY) ? 0 : 1;
}
