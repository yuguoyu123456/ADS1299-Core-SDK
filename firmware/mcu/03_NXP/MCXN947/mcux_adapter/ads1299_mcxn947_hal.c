#include "ads1299_mcxn947_hal.h"

static int adapter_fail(ads1299_mcxn947_context_t *ctx)
{
    if (ctx) ctx->fault = 1;
    return -1;
}

static int transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_mcxn947_context_t *ctx = (ads1299_mcxn947_context_t *)user;
    uint8_t zeros[32] = {0};
    uint8_t discard[32];
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;

    while (len) {
        size_t part = len > sizeof(zeros) ? sizeof(zeros) : len;
        lpspi_transfer_t xfer = {
            .txData = (uint8_t *)(tx ? tx : zeros),
            .rxData = rx ? rx : discard,
            .dataSize = part,
            .configFlags = kLPSPI_MasterPcsContinuous,
        };
        if (LPSPI_MasterTransferBlocking(ctx->spi, &xfer) != kStatus_Success)
            return adapter_fail(ctx);
        if (tx) tx += part;
        if (rx) rx += part;
        len -= part;
    }
    return 0;
}

static void write_pin(void *user, ads1299_platform_pin_t pin, int level)
{
    ads1299_mcxn947_context_t *ctx = (ads1299_mcxn947_context_t *)user;
    if (!ctx || (unsigned)pin >= ADS1299_PIN_DRDY) return;
    GPIO_PinWrite(ctx->gpio, ctx->ports[pin], ctx->pins[pin], level ? 1U : 0U);
}

static int read_pin(void *user, ads1299_platform_pin_t pin)
{
    ads1299_mcxn947_context_t *ctx = (ads1299_mcxn947_context_t *)user;
    if (!ctx || ctx->fault || (unsigned)pin > ADS1299_PIN_DRDY) return -1;
    return GPIO_PinRead(ctx->gpio, ctx->ports[pin], ctx->pins[pin]) ? 1 : 0;
}

static void delay_us(void *user, uint32_t us)
{
    ads1299_mcxn947_context_t *ctx = (ads1299_mcxn947_context_t *)user;
    if (ctx && ctx->delay_us && us) ctx->delay_us(ctx->delay_user, us);
}

int ads1299_mcxn947_make_hal(ads1299_mcxn947_context_t *ctx,
                             ads1299_platform_hal_t *hal)
{
    lpspi_master_config_t config;
    if (!ctx || !hal || !ctx->spi || !ctx->gpio || !ctx->source_hz ||
        !ctx->spi_hz || !ctx->poll_limit || !ctx->delay_us || ctx->fault)
        return -1;
    if (ctx->spi_hz > 4000000U) return -1;

    for (unsigned i = 0; i < 5; ++i) {
        if (ctx->pins[i] >= 32U) return -1;
        for (unsigned j = 0; j < i; ++j)
            if (ctx->ports[i] == ctx->ports[j] && ctx->pins[i] == ctx->pins[j])
                return -1;
    }

    LPSPI_MasterGetDefaultConfig(&config);
    config.cpol = kLPSPI_ClockPolarityActiveHigh;
    config.cpha = kLPSPI_ClockPhaseSecondEdge;
    config.direction = kLPSPI_MsbFirst;
    config.bitsPerFrame = 8U;
    config.baudRate = ctx->spi_hz;

    write_pin(ctx, ADS1299_PIN_CS, 1);
    write_pin(ctx, ADS1299_PIN_RESET, 1);
    write_pin(ctx, ADS1299_PIN_PWDN, 1);
    write_pin(ctx, ADS1299_PIN_START, 0);

    LPSPI_MasterInit(ctx->spi, &config, ctx->source_hz);
    ads1299_platform_hal_t result = {ctx, transfer, write_pin, read_pin, delay_us};
    *hal = result;
    return 0;
}
