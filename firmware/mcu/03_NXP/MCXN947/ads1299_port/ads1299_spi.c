#include "ads1299_spi.h"
#include "ads1299_gpio.h"
#include "ads1299_drdy.h"

static int port_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    return ads1299_mcu_spi_transfer(ctx, tx, rx, len);
}

int ads1299_mcu_spi_write(ads1299_mcu_port_t *ctx,
                          const uint8_t *data, size_t len) {
    if (!data) return -1;
    return ads1299_mcu_spi_transfer(ctx, data, NULL, len);
}

int ads1299_mcu_spi_read(ads1299_mcu_port_t *ctx,
                         uint8_t *data, size_t len) {
    if (!data) return -1;
    return ads1299_mcu_spi_transfer(ctx, NULL, data, len);
}

int ads1299_mcu_spi_transfer(ads1299_mcu_port_t *ctx,
                             const uint8_t *tx, uint8_t *rx, size_t len) {
    if (!ctx || !ctx->hal.spi_transfer || len == 0u || (!tx && !rx)) return -1;
    return ctx->hal.spi_transfer(ctx->hal.user, tx, rx, len);
}

static void port_delay(void *user, uint32_t us) {
    ads1299_mcu_port_t *ctx = (ads1299_mcu_port_t *)user;
    if (ctx && ctx->hal.delay_us) ctx->hal.delay_us(ctx->hal.user, us);
}

int ads1299_mcu_port_init(ads1299_mcu_port_t *ctx,
                          const ads1299_platform_hal_t *hal) {
    if (!ctx || !hal || !hal->spi_transfer || !hal->pin_write ||
        !hal->pin_read || !hal->delay_us) return -1;
    ctx->hal = *hal;
    return 0;
}

ads1299_port_t ads1299_mcu_make_port(ads1299_mcu_port_t *ctx) {
    ads1299_port_t port = {
        .user = ctx,
        .spi_transfer = port_spi,
        .cs_write = ads1299_port_cs_write,
        .reset_write = ads1299_port_reset_write,
        .pwdn_write = ads1299_port_pwdn_write,
        .start_write = ads1299_port_start_write,
        .drdy_read = ads1299_port_drdy_read,
        .delay_us = port_delay,
    };
    return port;
}
