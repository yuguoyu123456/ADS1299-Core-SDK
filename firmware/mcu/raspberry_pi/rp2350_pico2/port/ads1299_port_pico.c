#include "ads1299_port_pico.h"

static int pico_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_pico_ctx_t *ctx = (ads1299_pico_ctx_t *)user;
    if (!ctx || !ctx->spi) return -1;
    for (size_t i = 0; i < len; ++i) {
        uint8_t t = tx ? tx[i] : 0u;
        uint8_t r = 0u;
        if (spi_write_read_blocking(ctx->spi, &t, &r, 1) != 1) return -1;
        if (rx) rx[i] = r;
    }
    return 0;
}

static void pico_cs(void *user, int level) {
    ads1299_pico_ctx_t *ctx = (ads1299_pico_ctx_t *)user;
    gpio_put(ctx->cs_pin, level ? 1 : 0);
}

static void pico_reset(void *user, int level) {
    ads1299_pico_ctx_t *ctx = (ads1299_pico_ctx_t *)user;
    if (ctx->reset_pin >= 0) gpio_put((uint)ctx->reset_pin, level ? 1 : 0);
}

static void pico_start(void *user, int level) {
    ads1299_pico_ctx_t *ctx = (ads1299_pico_ctx_t *)user;
    if (ctx->start_pin >= 0) gpio_put((uint)ctx->start_pin, level ? 1 : 0);
}

static int pico_drdy(void *user) {
    ads1299_pico_ctx_t *ctx = (ads1299_pico_ctx_t *)user;
    return ctx->drdy_pin >= 0 ? gpio_get((uint)ctx->drdy_pin) : 1;
}

static void pico_delay(void *user, uint32_t us) {
    (void)user;
    sleep_us(us);
}

ads1299_port_t ads1299_pico_make_port(ads1299_pico_ctx_t *ctx) {
    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = pico_spi_transfer,
        .cs_write = pico_cs,
        .reset_write = pico_reset,
        .start_write = pico_start,
        .drdy_read = pico_drdy,
        .delay_us = pico_delay,
    };
    return p;
}
