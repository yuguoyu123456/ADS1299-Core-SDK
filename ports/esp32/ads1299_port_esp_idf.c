#include "ads1299_port_esp_idf.h"
#include "esp_rom_sys.h"
#include <string.h>

static int esp_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    if (!ctx || !ctx->spi || len == 0 || len > 256) return -1;

    uint8_t zeros[256] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8u;
    t.tx_buffer = tx ? tx : zeros;
    t.rx_buffer = rx;
    return spi_device_polling_transmit(ctx->spi, &t) == ESP_OK ? 0 : -1;
}

static void esp_cs(void *user, int level) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    gpio_set_level(ctx->cs_pin, level ? 1 : 0);
}

static void esp_reset(void *user, int level) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    if (ctx->reset_pin >= 0) gpio_set_level(ctx->reset_pin, level ? 1 : 0);
}

static void esp_start(void *user, int level) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    if (ctx->start_pin >= 0) gpio_set_level(ctx->start_pin, level ? 1 : 0);
}

static int esp_drdy(void *user) {
    ads1299_esp_idf_ctx_t *ctx = (ads1299_esp_idf_ctx_t *)user;
    return ctx->drdy_pin >= 0 ? gpio_get_level(ctx->drdy_pin) : 1;
}

static void esp_delay_us(void *user, uint32_t us) {
    (void)user;
    esp_rom_delay_us(us);
}

ads1299_port_t ads1299_esp_idf_make_port(ads1299_esp_idf_ctx_t *ctx) {
    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = esp_spi_transfer,
        .cs_write = esp_cs,
        .reset_write = esp_reset,
        .start_write = esp_start,
        .drdy_read = esp_drdy,
        .delay_us = esp_delay_us,
    };
    return p;
}
