/* Original adapter around official SPI APIs, no ADS1299 register logic. */
#include "ads1299_lpc55_hal.h"
static int fail(ads1299_lpc55_context_t *ctx) { ctx->fault=1; SPI_Enable(ctx->spi,false); return -1; }
static int fifo_error(ads1299_lpc55_context_t *ctx)
{ return (ctx->spi->FIFOSTAT&(SPI_FIFOSTAT_TXERR_MASK|SPI_FIFOSTAT_RXERR_MASK))!=0; }
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t len)
{
    ads1299_lpc55_context_t *ctx=user;
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;
    if (fifo_error(ctx) || ctx->spi->FIFOSTAT&SPI_FIFOSTAT_RXNOTEMPTY_MASK) return fail(ctx);
    while(len) {
        uint8_t zeros[32]={0},discard[32];
        size_t part=len>32 ? 32 : len;
        spi_transfer_t xfer={.txData=(uint8_t *)(tx ? tx : zeros),.rxData=rx ? rx : discard,
            .dataSize=part,.configFlags=kSPI_FrameAssert};
        if (SPI_MasterTransferBlocking(ctx->spi,&xfer)!=kStatus_Success || fifo_error(ctx)) return fail(ctx);
        /* FIFO empty is not shifter idle. Retain physical GPIO CS until idle. */
        uint32_t count=ctx->poll_limit;
        while (!(ctx->spi->STAT&SPI_STAT_MSTIDLE_MASK)) {
            if (fifo_error(ctx) || !--count) return fail(ctx);
        }
        if (tx) tx+=part;
        if (rx) rx+=part;
        len-=part;
    }
    return 0;
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_lpc55_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    GPIO_PinWrite(ctx->gpio,ctx->ports[pin],ctx->pins[pin],level ? 1 : 0);
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_lpc55_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return GPIO_PinRead(ctx->gpio,ctx->ports[pin],ctx->pins[pin]) ? 1 : 0;
}
static void delay(void *user,uint32_t us)
{
    ads1299_lpc55_context_t *ctx=user;
    if (us) ctx->delay_us(ctx->delay_user,us);
}
int ads1299_lpc55_make_hal(ads1299_lpc55_context_t *ctx,ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || !ctx->spi || !ctx->gpio || !ctx->source_hz || !ctx->spi_hz ||
        !ctx->poll_limit || !ctx->delay_us || ctx->fault) return -1;
    for(unsigned i=0;i<5;++i) {
        if (ctx->ports[i]>=2 || ctx->pins[i]>=32) return -1;
        for(unsigned j=0;j<i;++j)
            if (ctx->ports[i]==ctx->ports[j] && ctx->pins[i]==ctx->pins[j]) return -1;
    }
    uint64_t divisor=((uint64_t)ctx->source_hz+ctx->spi_hz-1)/ctx->spi_hz;
    if (!divisor || divisor>65536) return -1;
    spi_master_config_t config;
    SPI_MasterGetDefaultConfig(&config);
    config.phase=kSPI_ClockPhaseSecondEdge; config.polarity=kSPI_ClockPolarityActiveHigh;
    config.direction=kSPI_MsbFirst; config.dataWidth=kSPI_Data8Bits;
    config.baudRate_Bps=ctx->spi_hz;
    write_pin(ctx,ADS1299_PIN_CS,1); write_pin(ctx,ADS1299_PIN_RESET,1);
    write_pin(ctx,ADS1299_PIN_PWDN,1); write_pin(ctx,ADS1299_PIN_START,0);
    if (SPI_MasterInit(ctx->spi,&config,ctx->source_hz)!=kStatus_Success) return fail(ctx);
    /* Enforce a ceiling independently of SDK baud rounding policy. */
    ctx->spi->DIV=SPI_DIV_DIVVAL((uint32_t)divisor-1);
    ads1299_platform_hal_t result={ctx,transfer,write_pin,read_pin,delay};
    *hal=result; return 0;
}
