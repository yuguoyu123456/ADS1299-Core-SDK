#include "ads1299_port_stm32_hal.h"

static int stm32_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    if (!ctx || !ctx->hspi) return -1;

    for (size_t i = 0; i < len; ++i) {
        uint8_t txb = tx ? tx[i] : 0x00u;
        uint8_t rxb = 0u;
        if (HAL_SPI_TransmitReceive(ctx->hspi, &txb, &rxb, 1,
                                    ctx->spi_timeout_ms ? ctx->spi_timeout_ms : 10u) != HAL_OK) {
            return -1;
        }
        if (rx) rx[i] = rxb;
    }
    return 0;
}

static void stm32_cs_write(void *user, int level) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void stm32_reset_write(void *user, int level) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    if (ctx->reset_port) HAL_GPIO_WritePin(ctx->reset_port, ctx->reset_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void stm32_start_write(void *user, int level) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    if (ctx->start_port) HAL_GPIO_WritePin(ctx->start_port, ctx->start_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static int stm32_drdy_read(void *user) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    if (!ctx->drdy_port) return 1;
    return HAL_GPIO_ReadPin(ctx->drdy_port, ctx->drdy_pin) == GPIO_PIN_SET ? 1 : 0;
}

static void stm32_delay_us(void *user, uint32_t us) {
    ads1299_stm32_hal_ctx_t *ctx = (ads1299_stm32_hal_ctx_t *)user;
    if (ctx->delay_us_cb) {
        ctx->delay_us_cb(us);
    } else {
        HAL_Delay((us + 999u) / 1000u);
    }
}

ads1299_port_t ads1299_stm32_hal_make_port(ads1299_stm32_hal_ctx_t *ctx) {
    ads1299_port_t p = {
        .user = ctx,
        .spi_transfer = stm32_spi_transfer,
        .cs_write = stm32_cs_write,
        .reset_write = stm32_reset_write,
        .start_write = stm32_start_write,
        .drdy_read = stm32_drdy_read,
        .delay_us = stm32_delay_us,
    };
    return p;
}
