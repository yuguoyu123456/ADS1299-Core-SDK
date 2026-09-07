#include "ads1299_mspm0_hal.h"
#include <ti/driverlib/dl_common.h>

#ifndef ADS1299_MSPM0_POLL_LIMIT
#define ADS1299_MSPM0_POLL_LIMIT 100000u
#endif

static int mspm0_spi(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    ads1299_mspm0_ctx_t *ctx = (ads1299_mspm0_ctx_t *)user;
    size_t i;
    if (!ctx || !ctx->spi || !len || (!tx && !rx)) return -1;
    /* A stale RX byte indicates a previous incomplete transaction. Do not
     * silently shift it into a new ADS1299 frame. Caller must reinitialize. */
    if (!DL_SPI_isRXFIFOEmpty(ctx->spi)) return -1;
    for (i = 0; i < len; ++i) {
        uint32_t budget = ADS1299_MSPM0_POLL_LIMIT;
        while (DL_SPI_isTXFIFOFull(ctx->spi))
            if (budget-- == 0u) return -1;
        DL_SPI_transmitData8(ctx->spi, tx ? tx[i] : 0u);
        budget = ADS1299_MSPM0_POLL_LIMIT;
        while (DL_SPI_isRXFIFOEmpty(ctx->spi))
            if (budget-- == 0u) return -1;
        uint8_t value = DL_SPI_receiveData8(ctx->spi);
        if (rx) rx[i] = value;
    }
    uint32_t budget = ADS1299_MSPM0_POLL_LIMIT;
    while (DL_SPI_isBusy(ctx->spi))
        if (budget-- == 0u) return -1;
    return 0;
}

static ads1299_mspm0_gpio_t *mspm0_pin(ads1299_mspm0_ctx_t *ctx,
                                       ads1299_platform_pin_t pin) {
    switch (pin) {
        case ADS1299_PIN_CS: return &ctx->cs;
        case ADS1299_PIN_RESET: return &ctx->reset;
        case ADS1299_PIN_PWDN: return &ctx->pwdn;
        case ADS1299_PIN_START: return &ctx->start;
        case ADS1299_PIN_DRDY: return &ctx->drdy;
        default: return NULL;
    }
}

static void mspm0_write(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_mspm0_ctx_t *ctx = (ads1299_mspm0_ctx_t *)user;
    ads1299_mspm0_gpio_t *gpio;
    if (!ctx) return;
    gpio = mspm0_pin(ctx, pin);
    if (!gpio || !gpio->port || pin == ADS1299_PIN_DRDY) return;
    if (level) DL_GPIO_setPins(gpio->port, gpio->mask);
    else DL_GPIO_clearPins(gpio->port, gpio->mask);
}

static int mspm0_read(void *user, ads1299_platform_pin_t pin) {
    ads1299_mspm0_ctx_t *ctx = (ads1299_mspm0_ctx_t *)user;
    ads1299_mspm0_gpio_t *gpio;
    if (!ctx) return 1;
    gpio = mspm0_pin(ctx, pin);
    return (!gpio || !gpio->port) ? 1 :
        ((DL_GPIO_readPins(gpio->port, gpio->mask) & gpio->mask) != 0u);
}

static void mspm0_delay(void *user, uint32_t us) {
    ads1299_mspm0_ctx_t *ctx = (ads1299_mspm0_ctx_t *)user;
    uint64_t cycles;
    if (!ctx || !ctx->mclk_hz || !us) return;
    cycles = ((uint64_t)ctx->mclk_hz * us + 999999u) / 1000000u;
    while (cycles > UINT32_MAX) {
        DL_Common_delayCycles(UINT32_MAX);
        cycles -= UINT32_MAX;
    }
    /* TI documents zero as the maximum delay, not no delay. */
    if (cycles != 0u) DL_Common_delayCycles(cycles < 2u ? 2u : (uint32_t)cycles);
}

int ads1299_mspm0_make_hal(ads1299_platform_hal_t *hal,
                            ads1299_mspm0_ctx_t *ctx) {
    if (!hal || !ctx || !ctx->spi || !ctx->mclk_hz || !ctx->cs.port ||
        !ctx->reset.port || !ctx->pwdn.port || !ctx->start.port ||
        !ctx->drdy.port) return -1;
    const ads1299_mspm0_gpio_t *pins[] = {
        &ctx->cs, &ctx->reset, &ctx->pwdn, &ctx->start, &ctx->drdy
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i) {
        uint32_t mask = pins[i]->mask;
        if (mask == 0u || (mask & (mask - 1u)) != 0u) return -1;
        for (size_t j = 0; j < i; ++j)
            if (pins[i]->port == pins[j]->port && mask == pins[j]->mask)
                return -1;
    }
    *hal = (ads1299_platform_hal_t){ctx, mspm0_spi, mspm0_write,
                                    mspm0_read, mspm0_delay};
    return 0;
}
