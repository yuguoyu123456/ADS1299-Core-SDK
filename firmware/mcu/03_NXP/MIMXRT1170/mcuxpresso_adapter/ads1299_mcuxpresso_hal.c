#include "ads1299_mcuxpresso_hal.h"

#include "fsl_common.h"

#define ADS1299_MCUXPRESSO_CHUNK_BYTES 32u

static int nxp_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx,
                            size_t len) {
    ads1299_mcuxpresso_ctx_t *ctx = (ads1299_mcuxpresso_ctx_t *)user;
    size_t offset = 0u;

    if (!ctx || !ctx->lpspi || len == 0u || (!tx && !rx)) return -1;
    while (offset < len) {
        uint8_t zeros[ADS1299_MCUXPRESSO_CHUNK_BYTES] = {0};
        uint8_t discard[ADS1299_MCUXPRESSO_CHUNK_BYTES];
        size_t remaining = len - offset;
        size_t chunk = remaining < sizeof(zeros) ? remaining : sizeof(zeros);
        lpspi_transfer_t transfer = {
            .txData = (uint8_t *)(tx ? tx + offset : zeros),
            .rxData = rx ? rx + offset : discard,
            .dataSize = chunk,
            .configFlags = ctx->transfer_flags,
        };
        if (LPSPI_MasterTransferBlocking(ctx->lpspi, &transfer) != kStatus_Success)
            return -1;
        offset += chunk;
    }
    return 0;
}

static ads1299_mcuxpresso_gpio_t *nxp_pin(ads1299_mcuxpresso_ctx_t *ctx,
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

static void nxp_pin_write(void *user, ads1299_platform_pin_t pin, int level) {
    ads1299_mcuxpresso_ctx_t *ctx = (ads1299_mcuxpresso_ctx_t *)user;
    ads1299_mcuxpresso_gpio_t *gpio;
    if (!ctx) return;
    gpio = nxp_pin(ctx, pin);
    if (pin != ADS1299_PIN_DRDY && gpio && gpio->base && gpio->pin < 32u)
        GPIO_PinWrite(gpio->base, gpio->pin, level ? 1u : 0u);
}

static int nxp_pin_read(void *user, ads1299_platform_pin_t pin) {
    ads1299_mcuxpresso_ctx_t *ctx = (ads1299_mcuxpresso_ctx_t *)user;
    ads1299_mcuxpresso_gpio_t *gpio;
    if (!ctx) return 1;
    gpio = nxp_pin(ctx, pin);
    return (!gpio || !gpio->base || gpio->pin >= 32u) ? 1 :
        (int)GPIO_PinRead(gpio->base, gpio->pin);
}

static void nxp_delay_us(void *user, uint32_t us) {
    (void)user;
    SDK_DelayAtLeastUs(us, SystemCoreClock);
}

int ads1299_mcuxpresso_make_hal(ads1299_platform_hal_t *hal,
                                 ads1299_mcuxpresso_ctx_t *ctx) {
    if (!hal || !ctx || !ctx->lpspi || !ctx->cs.base || !ctx->reset.base ||
        !ctx->pwdn.base || !ctx->start.base || !ctx->drdy.base) return -1;
    const ads1299_mcuxpresso_gpio_t *pins[] = {
        &ctx->cs, &ctx->reset, &ctx->pwdn, &ctx->start, &ctx->drdy
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i) {
        if (pins[i]->pin >= 32u) return -1;
        for (size_t j = 0; j < i; ++j)
            if (pins[i]->base == pins[j]->base && pins[i]->pin == pins[j]->pin)
                return -1;
    }
    *hal = (ads1299_platform_hal_t){
        .user = ctx,
        .spi_transfer = nxp_spi_transfer,
        .pin_write = nxp_pin_write,
        .pin_read = nxp_pin_read,
        .delay_us = nxp_delay_us,
    };
    return 0;
}
