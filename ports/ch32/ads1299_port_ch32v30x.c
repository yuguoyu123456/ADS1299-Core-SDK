#include "ads1299_port_ch32v30x.h"

static int ch_wait(SPI_TypeDef *spi, uint16_t flag, FlagStatus wanted, uint32_t limit) {
    uint32_t n = 0;
    while (SPI_I2S_GetFlagStatus(spi, flag) != wanted) {
        if (++n >= limit) return -1;
    }
    return 0;
}

static int ch_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    if (!ctx || !ctx->spi) return -1;
    uint32_t limit = ctx->spin_limit ? ctx->spin_limit : 1000000u;
    for (size_t i = 0; i < len; ++i) {
        if (ch_wait(ctx->spi, SPI_I2S_FLAG_TXE, SET, limit)) return -1;
        SPI_I2S_SendData(ctx->spi, tx ? tx[i] : 0u);
        if (ch_wait(ctx->spi, SPI_I2S_FLAG_RXNE, SET, limit)) return -1;
        uint8_t r = (uint8_t)SPI_I2S_ReceiveData(ctx->spi);
        if (rx) rx[i] = r;
    }
    return 0;
}

static void ch_cs(void *user, int level) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    GPIO_WriteBit(ctx->cs_port, ctx->cs_pin, level ? Bit_SET : Bit_RESET);
}

static void ch_reset(void *user, int level) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    if (ctx->reset_port) GPIO_WriteBit(ctx->reset_port, ctx->reset_pin, level ? Bit_SET : Bit_RESET);
}

static void ch_start(void *user, int level) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    if (ctx->start_port) GPIO_WriteBit(ctx->start_port, ctx->start_pin, level ? Bit_SET : Bit_RESET);
}

static int ch_drdy(void *user) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    return ctx->drdy_port ? (GPIO_ReadInputDataBit(ctx->drdy_port, ctx->drdy_pin) == Bit_SET) : 1;
}

static void ch_delay(void *user, uint32_t us) {
    ads1299_ch32v30x_ctx_t *ctx = (ads1299_ch32v30x_ctx_t *)user;
    if (ctx->delay_us_cb) ctx->delay_us_cb(us);
}

ads1299_port_t ads1299_ch32v30x_make_port(ads1299_ch32v30x_ctx_t *ctx) {
    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = ch_spi_transfer,
        .cs_write = ch_cs,
        .reset_write = ch_reset,
        .start_write = ch_start,
        .drdy_read = ch_drdy,
        .delay_us = ch_delay,
    };
    return p;
}
