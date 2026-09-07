/* Original SDK adapter; FSP API contract: Renesas FSP v6.5.0 (BSD-3-Clause).
 * No ADS1299 commands, registers, channel or frame logic belongs here. */
#include "ads1299_fsp_hal.h"
#include <string.h>
#include <limits.h>

void ads1299_fsp_callback(spi_callback_args_t *args)
{
    if (args && args->p_context) {
        /* FSP 5.6 exposes const void *, 6.5 void *. This is our own mutable
         * context installed by open(), never arbitrary const user storage. */
        ads1299_fsp_context_t *ctx = (ads1299_fsp_context_t *) args->p_context;
        /* Never let a subsequent COMPLETE hide an earlier hardware error. */
        if (ctx->event == 0 || args->event != SPI_EVENT_TRANSFER_COMPLETE)
            ctx->event = (uint32_t) args->event;
    }
}

int ads1299_fsp_close(ads1299_fsp_context_t *ctx)
{
    if (!ctx) return -1;
    ctx->fault = 1;
    if (ctx->opened) {
        if (ctx->spi->p_api->close(ctx->spi->p_ctrl) != FSP_SUCCESS)
            return -1; /* retain ownership and static scratch storage */
        ctx->opened = 0;
    }
    return 0;
}

static int fail(ads1299_fsp_context_t *ctx)
{
    (void) ads1299_fsp_close(ctx);
    return -1;
}

static int transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_fsp_context_t *ctx = user;
    if (!ctx || !ctx->opened || ctx->fault || !len || (!tx && !rx)) return -1;
    while (len) {
        size_t n = len < sizeof(ctx->tx) ? len : sizeof(ctx->tx);
        if (tx) memcpy(ctx->tx, tx, n);
        else memset(ctx->tx, 0, n);
        ctx->event = 0;
        uint32_t polls_left = 100000U; /* independent failsafe for a stuck clock */
        uint32_t start = ctx->now_us(ctx->time_user);
        if (ctx->spi->p_api->writeRead(ctx->spi->p_ctrl, ctx->tx, ctx->rx,
                (uint32_t) n, SPI_BIT_WIDTH_8_BITS) != FSP_SUCCESS)
            return fail(ctx);
        while (ctx->event == 0) {
            if (--polls_left == 0 ||
                (uint32_t)(ctx->now_us(ctx->time_user) - start) >= ctx->timeout_us)
                return fail(ctx);
        }
        if (ctx->event != SPI_EVENT_TRANSFER_COMPLETE) return fail(ctx);
        if (rx) { memcpy(rx, ctx->rx, n); rx += n; }
        if (tx) tx += n;
        len -= n;
    }
    return 0;
}

static void pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    ads1299_fsp_context_t *ctx = user;
    if (!ctx || (unsigned)pin >= ADS1299_PIN_DRDY) return;
    /* Permit deasserting CS even after a transfer fault. */
    if (ctx->io->p_api->pinWrite(ctx->io->p_ctrl, ctx->pins[pin],
            level ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW) != FSP_SUCCESS)
        ctx->fault = 1;
}

static int pin_read(void *user, ads1299_platform_pin_t pin)
{
    ads1299_fsp_context_t *ctx = user;
    bsp_io_level_t level;
    if (!ctx || ctx->fault || !ctx->opened || (unsigned)pin > ADS1299_PIN_DRDY)
        return -1;
    if (ctx->io->p_api->pinRead(ctx->io->p_ctrl, ctx->pins[pin], &level) != FSP_SUCCESS) {
        ctx->fault = 1;
        return -1;
    }
    return level == BSP_IO_LEVEL_HIGH ? 1 : 0;
}

static void delay(void *user, uint32_t us)
{
    ads1299_fsp_context_t *ctx = user;
    if (us) ctx->delay_us(ctx->time_user, us);
}

int ads1299_fsp_open(ads1299_fsp_context_t *ctx, ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || ctx->opened || !ctx->spi || !ctx->io ||
        !ctx->spi->p_ctrl || !ctx->spi->p_cfg || !ctx->spi->p_api ||
        !ctx->io->p_ctrl || !ctx->io->p_api || !ctx->now_us || !ctx->delay_us ||
        !ctx->timeout_us || ctx->timeout_us > INT32_MAX) return -1;
    spi_api_t const *api = ctx->spi->p_api;
    spi_cfg_t const *cfg = ctx->spi->p_cfg;
    if (!api->open || !api->close || !api->writeRead ||
        !ctx->io->p_api->pinRead || !ctx->io->p_api->pinWrite ||
        cfg->operating_mode != SPI_MODE_MASTER ||
        cfg->clk_phase != SPI_CLK_PHASE_EDGE_EVEN ||
        cfg->clk_polarity != SPI_CLK_POLARITY_LOW ||
        cfg->bit_order != SPI_BIT_ORDER_MSB_FIRST || !cfg->p_extend ||
        cfg->p_transfer_rx || cfg->p_transfer_tx || cfg->rxi_irq < 0 ||
        cfg->txi_irq < 0 || cfg->tei_irq < 0 || cfg->eri_irq < 0) return -1;
    for (unsigned i = 0; i < 5; ++i)
        for (unsigned j = 0; j < i; ++j)
            if (ctx->pins[i] == ctx->pins[j]) return -1;
    ctx->fault = 0;
    ctx->event = 0;
    pin_write(ctx, ADS1299_PIN_CS, 1);
    pin_write(ctx, ADS1299_PIN_RESET, 1);
    pin_write(ctx, ADS1299_PIN_PWDN, 1);
    pin_write(ctx, ADS1299_PIN_START, 0);
    if (ctx->fault) return -1;
    ctx->config = *cfg;
    ctx->config.p_callback = ads1299_fsp_callback;
    ctx->config.p_context = ctx;
    if (api->open(ctx->spi->p_ctrl, &ctx->config) != FSP_SUCCESS) {
        ctx->fault = 1;
        return -1;
    }
    ctx->opened = 1;
    ads1299_platform_hal_t result = {ctx, transfer, pin_write, pin_read, delay};
    *hal = result;
    return 0;
}
