/* Original hardware-only implementation; no ADS1299 register/command logic. */
#include "ads1299_same54_hal.h"

static int stop(ads1299_same54_context_t *ctx)
{
    ctx->fault=1;
    ctx->spi->SPIM.SERCOM_INTENCLR=SERCOM_SPIM_INTENCLR_Msk;
    ctx->spi->SPIM.SERCOM_CTRLA &= ~SERCOM_SPIM_CTRLA_ENABLE_Msk;
    /* Do not wait indefinitely for disable synchronization on a failed clock.
     * No ISR/DMA owns caller memory; board reset is required before reuse. */
    return -1;
}
static int sync_wait(ads1299_same54_context_t *ctx)
{
    for (uint32_t n=ctx->poll_limit;n;--n)
        if (ctx->spi->SPIM.SERCOM_SYNCBUSY==0) return 0;
    return stop(ctx);
}
static int flag_wait(ads1299_same54_context_t *ctx, uint8_t flag)
{
    for (uint32_t n=ctx->poll_limit;n;--n) {
        if ((ctx->spi->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_BUFOVF_Msk) ||
            (ctx->spi->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_ERROR_Msk))
            return stop(ctx);
        if (ctx->spi->SPIM.SERCOM_INTFLAG & flag) return 0;
    }
    return stop(ctx);
}
static int transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len)
{
    ads1299_same54_context_t *ctx=user;
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;
    if (ctx->spi->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk)
        return stop(ctx); /* stale/unexpected receive data */
    while (len--) {
        if (flag_wait(ctx,SERCOM_SPIM_INTFLAG_DRE_Msk)) return -1;
        ctx->spi->SPIM.SERCOM_DATA=tx ? *tx++ : 0;
        if (flag_wait(ctx,SERCOM_SPIM_INTFLAG_RXC_Msk)) return -1;
        uint8_t value=(uint8_t)ctx->spi->SPIM.SERCOM_DATA;
        if (rx) *rx++=value;
    }
    /* DRE alone is insufficient: final byte must leave the shift register. */
    return flag_wait(ctx,SERCOM_SPIM_INTFLAG_TXC_Msk);
}
static void pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    ads1299_same54_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    uint32_t id=ctx->pins[pin], mask=1UL<<(id%32U);
    if (level) PORT_REGS->GROUP[id/32U].PORT_OUTSET=mask;
    else PORT_REGS->GROUP[id/32U].PORT_OUTCLR=mask;
}
static int pin_read(void *user, ads1299_platform_pin_t pin)
{
    ads1299_same54_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    uint32_t id=ctx->pins[pin];
    return (PORT_REGS->GROUP[id/32U].PORT_IN & (1UL<<(id%32U))) ? 1 : 0;
}
static void delay(void *user, uint32_t us)
{
    ads1299_same54_context_t *ctx=user;
    if (us) ctx->delay_us(ctx->delay_user,us);
}
int ads1299_same54_make_hal(ads1299_same54_context_t *ctx, ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || !ctx->spi || !ctx->source_hz || !ctx->spi_hz ||
        !ctx->poll_limit || !ctx->delay_us || ctx->fault) return -1;
    /* Ceiling division avoids exceeding requested SCK. BAUD range 0..255. */
    uint64_t divisor=((uint64_t)ctx->source_hz+2ULL*ctx->spi_hz-1)/(2ULL*ctx->spi_hz);
    if (divisor<1 || divisor>256) return -1;
    for (unsigned i=0;i<5;++i) {
        if (ctx->pins[i]>=128) return -1;
        for (unsigned j=0;j<i;++j) if (ctx->pins[i]==ctx->pins[j]) return -1;
    }
    pin_write(ctx,ADS1299_PIN_CS,1);
    pin_write(ctx,ADS1299_PIN_RESET,1);
    pin_write(ctx,ADS1299_PIN_PWDN,1);
    pin_write(ctx,ADS1299_PIN_START,0);
    ctx->spi->SPIM.SERCOM_INTENCLR=SERCOM_SPIM_INTENCLR_Msk;
    ctx->spi->SPIM.SERCOM_CTRLA &= ~SERCOM_SPIM_CTRLA_ENABLE_Msk;
    if (sync_wait(ctx)) return -1;
    ctx->spi->SPIM.SERCOM_CTRLA=SERCOM_SPIM_CTRLA_MODE_SPI_MASTER |
        SERCOM_SPIM_CTRLA_DOPO_PAD0 | SERCOM_SPIM_CTRLA_DIPO_PAD3 |
        SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW | SERCOM_SPIM_CTRLA_CPHA_TRAILING_EDGE |
        SERCOM_SPIM_CTRLA_DORD_MSB;
    ctx->spi->SPIM.SERCOM_CTRLB=SERCOM_SPIM_CTRLB_CHSIZE_8_BIT | SERCOM_SPIM_CTRLB_RXEN_Msk;
    if (sync_wait(ctx)) return -1;
    ctx->spi->SPIM.SERCOM_BAUD=(uint8_t)(divisor-1);
    ctx->spi->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;
    if (sync_wait(ctx)) return -1;
    ads1299_platform_hal_t result={ctx,transfer,pin_write,pin_read,delay};
    *hal=result;
    return 0;
}
