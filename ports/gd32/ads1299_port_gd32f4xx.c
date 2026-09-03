#include "ads1299_port_gd32f4xx.h"

static int wait_flag(uint32_t spi, uint32_t flag, FlagStatus wanted, uint32_t limit) {
    uint32_t n = 0;
    while (spi_i2s_flag_get(spi, flag) != wanted) {
        if (++n >= limit) return -1;
    }
    return 0;
}

static int gd_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    if (!ctx) return -1;
    uint32_t limit = ctx->spin_limit ? ctx->spin_limit : 1000000u;
    for (size_t i = 0; i < len; ++i) {
        if (wait_flag(ctx->spi_periph, SPI_FLAG_TBE, SET, limit)) return -1;
        spi_i2s_data_transmit(ctx->spi_periph, tx ? tx[i] : 0u);
        if (wait_flag(ctx->spi_periph, SPI_FLAG_RBNE, SET, limit)) return -1;
        uint8_t r = (uint8_t)spi_i2s_data_receive(ctx->spi_periph);
        if (rx) rx[i] = r;
    }
    return 0;
}

static void gd_cs(void *user, int level) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    gpio_bit_write(ctx->cs_port, ctx->cs_pin, level ? SET : RESET);
}

static void gd_reset(void *user, int level) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    if (ctx->reset_port) gpio_bit_write(ctx->reset_port, ctx->reset_pin, level ? SET : RESET);
}

static void gd_start(void *user, int level) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    if (ctx->start_port) gpio_bit_write(ctx->start_port, ctx->start_pin, level ? SET : RESET);
}

static int gd_drdy(void *user) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    return ctx->drdy_port ? (gpio_input_bit_get(ctx->drdy_port, ctx->drdy_pin) == SET) : 1;
}

static void gd_delay(void *user, uint32_t us) {
    ads1299_gd32f4xx_ctx_t *ctx = (ads1299_gd32f4xx_ctx_t *)user;
    if (ctx->delay_us_cb) ctx->delay_us_cb(us);
}

ads1299_port_t ads1299_gd32f4xx_make_port(ads1299_gd32f4xx_ctx_t *ctx) {
    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = gd_spi_transfer,
        .cs_write = gd_cs,
        .reset_write = gd_reset,
        .start_write = gd_start,
        .drdy_read = gd_drdy,
        .delay_us = gd_delay,
    };
    return p;
}
