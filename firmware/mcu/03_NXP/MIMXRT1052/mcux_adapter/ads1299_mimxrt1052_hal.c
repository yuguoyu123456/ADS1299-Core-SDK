#include "ads1299_mimxrt1052_hal.h"

#include <limits.h>
#include <stddef.h>

#define ADS1299_MIMXRT1052_MAX_BRINGUP_SPI_HZ 4000000u

static int gpio_valid(const ads1299_mimxrt1052_gpio_t *p)
{
    return p != NULL && p->gpio != NULL && p->pin < 32u;
}

static int gpio_same(const ads1299_mimxrt1052_gpio_t *a,
                     const ads1299_mimxrt1052_gpio_t *b)
{
    return a->gpio == b->gpio && a->pin == b->pin;
}

static int adapter_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_mimxrt1052_hal_context_t *ctx = (ads1299_mimxrt1052_hal_context_t *)user;
    lpspi_transfer_t xfer;
    status_t status;

    if (ctx == NULL || ctx->lpspi == NULL || len == 0u || len > UINT32_MAX) {
        return -1;
    }

    xfer.txData = (uint8_t *)tx;
    xfer.rxData = rx;
    xfer.dataSize = (uint32_t)len;
    xfer.configFlags = kLPSPI_MasterPcsContinuous;
    status = LPSPI_MasterTransferBlocking(ctx->lpspi, &xfer);
    return status == kStatus_Success ? 0 : -1;
}

static ads1299_mimxrt1052_gpio_t *pin_for(ads1299_mimxrt1052_hal_context_t *ctx,
                                          ads1299_platform_pin_t pin)
{
    switch (pin) {
    case ADS1299_PIN_CS: return &ctx->cs;
    case ADS1299_PIN_RESET: return &ctx->reset;
    case ADS1299_PIN_PWDN: return &ctx->pwdn;
    case ADS1299_PIN_START: return &ctx->start;
    case ADS1299_PIN_DRDY: return &ctx->drdy;
    default: return NULL;
    }
}

static void adapter_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    ads1299_mimxrt1052_hal_context_t *ctx = (ads1299_mimxrt1052_hal_context_t *)user;
    ads1299_mimxrt1052_gpio_t *p = ctx != NULL ? pin_for(ctx, pin) : NULL;
    if (gpio_valid(p)) {
        GPIO_PinWrite(p->gpio, p->pin, level != 0 ? 1u : 0u);
    }
}

static int adapter_pin_read(void *user, ads1299_platform_pin_t pin)
{
    ads1299_mimxrt1052_hal_context_t *ctx = (ads1299_mimxrt1052_hal_context_t *)user;
    ads1299_mimxrt1052_gpio_t *p = ctx != NULL ? pin_for(ctx, pin) : NULL;
    return gpio_valid(p) ? (int)GPIO_PinRead(p->gpio, p->pin) : -1;
}

static void adapter_delay_us(void *user, uint32_t us)
{
    ads1299_mimxrt1052_hal_context_t *ctx = (ads1299_mimxrt1052_hal_context_t *)user;
    if (ctx != NULL && ctx->delay_us != NULL) {
        ctx->delay_us(us);
    }
}

int ads1299_mimxrt1052_hal_init(ads1299_mimxrt1052_hal_context_t *ctx)
{
    lpspi_master_config_t config;
    const ads1299_mimxrt1052_gpio_t *pins[5];
    size_t i;
    size_t j;

    if (ctx == NULL || ctx->lpspi == NULL || ctx->source_clock_hz == 0u ||
        ctx->baud_hz == 0u || ctx->baud_hz > ADS1299_MIMXRT1052_MAX_BRINGUP_SPI_HZ ||
        ctx->delay_us == NULL) {
        return -1;
    }

    pins[0] = &ctx->cs; pins[1] = &ctx->reset; pins[2] = &ctx->pwdn;
    pins[3] = &ctx->start; pins[4] = &ctx->drdy;
    for (i = 0u; i < 5u; ++i) {
        if (!gpio_valid(pins[i])) return -2;
        for (j = i + 1u; j < 5u; ++j) {
            if (gpio_same(pins[i], pins[j])) return -3;
        }
    }

    LPSPI_MasterGetDefaultConfig(&config);
    config.baudRate = ctx->baud_hz;
    config.bitsPerFrame = 8u;
    config.cpol = kLPSPI_ClockPolarityActiveHigh;
    config.cpha = kLPSPI_ClockPhaseSecondEdge;
    config.direction = kLPSPI_MsbFirst;
    if (LPSPI_MasterInit(ctx->lpspi, &config, ctx->source_clock_hz) != kStatus_Success) {
        return -4;
    }
    return 0;
}

ads1299_platform_hal_t ads1299_mimxrt1052_make_hal(ads1299_mimxrt1052_hal_context_t *ctx)
{
    ads1299_platform_hal_t hal;
    hal.user = ctx;
    hal.spi_transfer = adapter_spi_transfer;
    hal.pin_write = adapter_pin_write;
    hal.pin_read = adapter_pin_read;
    hal.delay_us = adapter_delay_us;
    return hal;
}
