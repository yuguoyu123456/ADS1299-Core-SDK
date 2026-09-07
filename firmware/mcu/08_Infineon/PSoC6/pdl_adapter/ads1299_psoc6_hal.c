/* Original adaptation of official PDL APIs; ADS1299 behavior stays in Core. */
#include "ads1299_psoc6_hal.h"
static int fail(ads1299_psoc6_context_t *ctx)
{
    ctx->fault=1;
    Cy_SCB_SPI_Disable(ctx->spi,NULL);
    return -1;
}
static int rx_error(ads1299_psoc6_context_t *ctx)
{
    return (Cy_SCB_SPI_GetRxFifoStatus(ctx->spi)&CY_SCB_SPI_RX_OVERFLOW)!=0;
}
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t len)
{
    ads1299_psoc6_context_t *ctx=user;
    if (!ctx || ctx->fault || !len || (!tx && !rx)) return -1;
    if (Cy_SCB_SPI_GetNumInRxFifo(ctx->spi)) return fail(ctx);
    while (len--) {
        uint32_t count=ctx->poll_limit;
        while (!Cy_SCB_SPI_Write(ctx->spi,tx ? *tx : 0)) {
            if (rx_error(ctx) || --count==0) return fail(ctx);
        }
        if (tx) ++tx;
        count=ctx->poll_limit;
        while (!Cy_SCB_SPI_GetNumInRxFifo(ctx->spi)) {
            if (rx_error(ctx) || --count==0) return fail(ctx);
        }
        if (rx_error(ctx)) return fail(ctx);
        uint32_t value=Cy_SCB_SPI_Read(ctx->spi);
        if (value==CY_SCB_SPI_RX_NO_DATA) return fail(ctx);
        if (rx) *rx++=(uint8_t)value;
    }
    uint32_t count=ctx->poll_limit;
    while (!Cy_SCB_SPI_IsTxComplete(ctx->spi)) {
        if (rx_error(ctx) || --count==0) return fail(ctx);
    }
    return rx_error(ctx) ? fail(ctx) : 0;
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_psoc6_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    Cy_GPIO_Write(ctx->gpio[pin],ctx->pins[pin],level ? 1U : 0U);
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_psoc6_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return Cy_GPIO_Read(ctx->gpio[pin],ctx->pins[pin]) ? 1 : 0;
}
static void delay(void *user,uint32_t us)
{
    ads1299_psoc6_context_t *ctx=user;
    if (us) ctx->delay_us(ctx->delay_user,us);
}
int ads1299_psoc6_make_hal(ads1299_psoc6_context_t *ctx,
                         const cy_stc_scb_spi_config_t *config,
                         ads1299_platform_hal_t *hal)
{
    if (!ctx || !config || !hal || !ctx->spi || !ctx->poll_limit ||
        !ctx->delay_us || ctx->fault) return -1;
    if (config->spiMode!=CY_SCB_SPI_MASTER || config->subMode!=CY_SCB_SPI_MOTOROLA ||
        config->sclkMode!=CY_SCB_SPI_CPHA1_CPOL0 || !config->enableMsbFirst ||
        config->rxDataWidth!=8 || config->txDataWidth!=8 ||
        config->oversample<4 || config->oversample>16 ||
        config->enableFreeRunSclk || config->enableMisoLateSample ||
        config->rxFifoIntEnableMask || config->txFifoIntEnableMask ||
        config->masterSlaveIntEnableMask) return -1;
    for (unsigned i=0;i<5;++i) {
        if (!ctx->gpio[i] || ctx->pins[i]>=8) return -1;
        for (unsigned j=0;j<i;++j)
            if (ctx->gpio[i]==ctx->gpio[j] && ctx->pins[i]==ctx->pins[j]) return -1;
    }
    write_pin(ctx,ADS1299_PIN_CS,1); write_pin(ctx,ADS1299_PIN_RESET,1);
    write_pin(ctx,ADS1299_PIN_PWDN,1); write_pin(ctx,ADS1299_PIN_START,0);
    if (Cy_SCB_SPI_Init(ctx->spi,config,NULL)!=CY_SCB_SPI_SUCCESS) return fail(ctx);
    Cy_SCB_SPI_Enable(ctx->spi);
    ads1299_platform_hal_t result={ctx,transfer,write_pin,read_pin,delay};
    *hal=result;
    return 0;
}
