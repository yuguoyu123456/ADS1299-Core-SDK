/* Original bounded polling transport; chip behavior remains in shared Core. */
#include "ads1299_g4_hal.h"
static int fail(ads1299_g4_context_t *ctx)
{
    ctx->fault=1;
    ctx->spi->CR1 &= ~SPI_CR1_SPE;
    return -1;
}
static int wait_status(ads1299_g4_context_t *ctx,uint32_t mask,int set)
{
    uint32_t count=ctx->poll_limit;
    do {
        uint32_t status=ctx->spi->SR;
        if (status&(SPI_SR_OVR|SPI_SR_MODF|SPI_SR_FRE)) return fail(ctx);
        if (set ? (status&mask)==mask : !(status&mask)) return 0;
    } while (--count);
    return fail(ctx);
}
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t len)
{
    ads1299_g4_context_t *ctx=user;
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;
    if (ctx->spi->SR & (SPI_SR_RXNE|SPI_SR_FRLVL|SPI_SR_FTLVL|SPI_SR_BSY)) return fail(ctx);
    while (len--) {
        if (wait_status(ctx,SPI_SR_TXE,1)) return -1;
        *(__IO uint8_t *)&ctx->spi->DR=tx ? *tx++ : 0;
        if (wait_status(ctx,SPI_SR_RXNE,1)) return -1;
        uint8_t value=*(__IO uint8_t *)&ctx->spi->DR;
        if (rx) *rx++=value;
    }
    return wait_status(ctx,SPI_SR_FTLVL|SPI_SR_BSY,0);
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_g4_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    ctx->gpio[pin]->BSRR=level ? ctx->pins[pin] : (uint32_t)ctx->pins[pin]<<16;
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_g4_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return (ctx->gpio[pin]->IDR&ctx->pins[pin]) ? 1 : 0;
}
static void delay(void *user,uint32_t us)
{
    ads1299_g4_context_t *ctx=user;
    if (us) ctx->delay_us(ctx->delay_user,us);
}
int ads1299_g4_make_hal(ads1299_g4_context_t *ctx,ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || !ctx->spi || !ctx->source_hz || !ctx->spi_hz ||
        !ctx->poll_limit || !ctx->delay_us || ctx->fault) return -1;
    for(unsigned i=0;i<5;++i) {
        if (!ctx->gpio[i] || !ctx->pins[i] || (ctx->pins[i]&(ctx->pins[i]-1))) return -1;
        for(unsigned j=0;j<i;++j)
            if (ctx->gpio[i]==ctx->gpio[j] && ctx->pins[i]==ctx->pins[j]) return -1;
    }
    unsigned br=0; uint32_t divisor=2;
    while ((uint64_t)ctx->spi_hz*divisor<ctx->source_hz && br<7) { ++br; divisor*=2; }
    if ((uint64_t)ctx->spi_hz*divisor<ctx->source_hz) return -1;
    /* Refuse to seize a running peripheral. Board must reset it before binding. */
    if (ctx->spi->CR1&SPI_CR1_SPE) return -1;
    write_pin(ctx,ADS1299_PIN_CS,1); write_pin(ctx,ADS1299_PIN_RESET,1);
    write_pin(ctx,ADS1299_PIN_PWDN,1); write_pin(ctx,ADS1299_PIN_START,0);
    ctx->spi->CR1=SPI_CR1_MSTR|SPI_CR1_SSM|SPI_CR1_SSI|SPI_CR1_CPHA|(br<<SPI_CR1_BR_Pos);
    ctx->spi->CR2=(7U<<SPI_CR2_DS_Pos)|SPI_CR2_FRXTH;
    ctx->spi->CR1 |= SPI_CR1_SPE;
    ads1299_platform_hal_t result={ctx,transfer,write_pin,read_pin,delay};
    *hal=result;
    return 0;
}
