/* Original packet-counted U575 SPI transport; no ADS1299 register logic. */
#include "ads1299_u5_hal.h"
static int fail(ads1299_u5_context_t *ctx)
{
    ctx->fault=1;
    ctx->spi->CR1 &= ~SPI_CR1_SPE;
    return -1;
}
static int wait_set(ads1299_u5_context_t *ctx,uint32_t flag)
{
    uint32_t count=ctx->poll_limit;
    do {
        uint32_t sr=ctx->spi->SR;
        if (sr&(SPI_SR_OVR|SPI_SR_UDR|SPI_SR_MODF|SPI_SR_TIFRE)) return fail(ctx);
        if (sr&flag) return 0;
    } while (--count);
    return fail(ctx);
}
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t len)
{
    ads1299_u5_context_t *ctx=user;
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;
    if (ctx->spi->CR1&SPI_CR1_SPE) return fail(ctx);
    while (len) {
        uint32_t part=len>65535U ? 65535U : (uint32_t)len;
        if (ctx->spi->SR&(SPI_SR_RXP|SPI_SR_RXPLVL|SPI_SR_RXWNE)) return fail(ctx);
        ctx->spi->IFCR=SPI_IFCR_EOTC|SPI_IFCR_TXTFC;
        ctx->spi->CR2=part;
        ctx->spi->CR1 |= SPI_CR1_SPE;
        ctx->spi->CR1 |= SPI_CR1_CSTART;
        for(uint32_t i=0;i<part;++i) {
            if (wait_set(ctx,SPI_SR_TXP)) return -1;
            *(__IO uint8_t *)&ctx->spi->TXDR=tx ? *tx++ : 0;
            if (wait_set(ctx,SPI_SR_RXP)) return -1;
            uint8_t value=*(__IO uint8_t *)&ctx->spi->RXDR;
            if (rx) *rx++=value;
        }
        if (wait_set(ctx,SPI_SR_EOT)) return -1;
        /* ES0499: do not truncate the last SCK edge by disabling at EOT.
         * Conservative two actual SCK periods, rounded up to microseconds.
         * Board delay must meet this minimum and latch ctx->fault on failure. */
        ctx->delay_us(ctx->delay_user,ctx->eot_delay_us);
        if (ctx->fault) return fail(ctx);
        ctx->spi->IFCR=SPI_IFCR_EOTC|SPI_IFCR_TXTFC;
        ctx->spi->CR1 &= ~SPI_CR1_SPE;
        len-=part;
    }
    return 0;
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_u5_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    ctx->gpio[pin]->BSRR=level ? ctx->pins[pin] : (uint32_t)ctx->pins[pin]<<16;
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_u5_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return (ctx->gpio[pin]->IDR&ctx->pins[pin]) ? 1 : 0;
}
static void delay(void *user,uint32_t us)
{
    ads1299_u5_context_t *ctx=user;
    if (us) ctx->delay_us(ctx->delay_user,us);
}
int ads1299_u5_make_hal(ads1299_u5_context_t *ctx,ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || !ctx->spi || !ctx->source_hz || !ctx->spi_hz ||
        !ctx->poll_limit || !ctx->delay_us || ctx->fault) return -1;
    if (ctx->spi!=SPI1 || ctx->spi->CR1&SPI_CR1_SPE) return -1;
    for(unsigned i=0;i<5;++i) {
        if (!ctx->gpio[i] || !ctx->pins[i] || (ctx->pins[i]&(ctx->pins[i]-1))) return -1;
        for(unsigned j=0;j<i;++j)
            if (ctx->gpio[i]==ctx->gpio[j] && ctx->pins[i]==ctx->pins[j]) return -1;
    }
    unsigned br=0; uint32_t divisor=2;
    while ((uint64_t)ctx->spi_hz*divisor<ctx->source_hz && br<7) { ++br; divisor*=2; }
    if ((uint64_t)ctx->spi_hz*divisor<ctx->source_hz) return -1;
    ctx->eot_delay_us=(uint32_t)(((uint64_t)2000000U*divisor+
                                ctx->source_hz-1U)/ctx->source_hz);
    write_pin(ctx,ADS1299_PIN_CS,1); write_pin(ctx,ADS1299_PIN_RESET,1);
    write_pin(ctx,ADS1299_PIN_PWDN,1); write_pin(ctx,ADS1299_PIN_START,0);
    ctx->spi->CR1=SPI_CR1_SSI;
    ctx->spi->CFG1=7U|(br<<SPI_CFG1_MBR_Pos); /* 8 bits, one-byte FIFO threshold */
    ctx->spi->CFG2=SPI_CFG2_MASTER|SPI_CFG2_SSM|SPI_CFG2_CPHA|SPI_CFG2_AFCNTR;
    ctx->spi->IER=0; ctx->spi->AUTOCR=0;
    ads1299_platform_hal_t result={ctx,transfer,write_pin,read_pin,delay};
    *hal=result;
    return 0;
}
