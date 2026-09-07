#include "ads1299_port_wch.h"
#include "debug.h"

#include <stddef.h>

static int wch_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_wch_ctx_t *ctx = (ads1299_wch_ctx_t *)user;
    if (!ctx || !ctx->spi || len == 0u) {
        return -1;
    }

    for (size_t i = 0; i < len; ++i) {
        while (SPI_I2S_GetFlagStatus(ctx->spi, SPI_I2S_FLAG_TXE) == RESET) {
        }

        SPI_I2S_SendData(ctx->spi, tx ? tx[i] : 0x00u);

        while (SPI_I2S_GetFlagStatus(ctx->spi, SPI_I2S_FLAG_RXNE) == RESET) {
        }

        const uint8_t value = (uint8_t)SPI_I2S_ReceiveData(ctx->spi);
        if (rx) {
            rx[i] = value;
        }
    }

    while (SPI_I2S_GetFlagStatus(ctx->spi, SPI_I2S_FLAG_BSY) != RESET) {
    }

    return 0;
}

static void wch_gpio_write(GPIO_TypeDef *port, uint16_t pin, int level)
{
    if (!port || pin == 0u) {
        return;
    }
    GPIO_WriteBit(port, pin, level ? Bit_SET : Bit_RESET);
}

static void wch_cs_write(void *user, int level)
{
    ads1299_wch_ctx_t *ctx = (ads1299_wch_ctx_t *)user;
    if (ctx) {
        wch_gpio_write(ctx->cs_port, ctx->cs_pin, level);
    }
}

static void wch_reset_write(void *user, int level)
{
    ads1299_wch_ctx_t *ctx = (ads1299_wch_ctx_t *)user;
    if (ctx) {
        wch_gpio_write(ctx->reset_port, ctx->reset_pin, level);
    }
}

static void wch_start_write(void *user, int level)
{
    ads1299_wch_ctx_t *ctx = (ads1299_wch_ctx_t *)user;
    if (ctx) {
        wch_gpio_write(ctx->start_port, ctx->start_pin, level);
    }
}

static int wch_drdy_read(void *user)
{
    ads1299_wch_ctx_t *ctx = (ads1299_wch_ctx_t *)user;
    if (!ctx || !ctx->drdy_port || ctx->drdy_pin == 0u) {
        return 1;
    }
    return GPIO_ReadInputDataBit(ctx->drdy_port, ctx->drdy_pin) ? 1 : 0;
}

static void wch_delay_us(void *user, uint32_t us)
{
    (void)user;
    Delay_Us(us);
}

ads1299_port_t ads1299_wch_make_port(ads1299_wch_ctx_t *ctx)
{
    ads1299_port_t port = {
        .user = ctx,
        .spi_transfer = wch_spi_transfer,
        .cs_write = wch_cs_write,
        .reset_write = wch_reset_write,
        .start_write = wch_start_write,
        .drdy_read = wch_drdy_read,
        .delay_us = wch_delay_us,
    };
    return port;
}
