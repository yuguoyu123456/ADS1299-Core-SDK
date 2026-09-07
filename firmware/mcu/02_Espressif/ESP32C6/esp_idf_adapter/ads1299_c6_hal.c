/* Original bounded-wait IDF binding; ADS1299 logic stays in Core. */
#include "ads1299_c6_hal.h"
#include "esp_rom_sys.h"
#include <string.h>
static int fail(ads1299_c6_context_t *ctx) { ctx->fault=1; return -1; }
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t len)
{
    ads1299_c6_context_t *ctx=user;
    if (!ctx || ctx->fault || ctx->in_flight || !len || (!tx && !rx)) return -1;
    while(len) {
        size_t part=len>sizeof ctx->tx ? sizeof ctx->tx : len;
        if (tx) memcpy(ctx->tx,tx,part); else memset(ctx->tx,0,part);
        memset(&ctx->transaction,0,sizeof ctx->transaction);
        ctx->transaction.length=part*8;
        ctx->transaction.tx_buffer=ctx->tx; ctx->transaction.rx_buffer=ctx->rx;
        if (spi_device_queue_trans(ctx->spi,&ctx->transaction,ctx->timeout_ticks)!=ESP_OK) return fail(ctx);
        ctx->in_flight=1;
        spi_transaction_t *done=NULL;
        if (spi_device_get_trans_result(ctx->spi,&done,ctx->timeout_ticks)!=ESP_OK) return fail(ctx);
        if (done!=&ctx->transaction) return fail(ctx);
        ctx->in_flight=0;
        if (rx) { memcpy(rx,ctx->rx,part); rx+=part; }
        if (tx) tx+=part;
        len-=part;
    }
    return 0;
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_c6_context_t *ctx=user;
    if (!ctx || (unsigned)pin>=ADS1299_PIN_DRDY) return;
    if (gpio_set_level(ctx->pins[pin],level ? 1 : 0)!=ESP_OK) ctx->fault=1;
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_c6_context_t *ctx=user;
    if (!ctx || ctx->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return gpio_get_level(ctx->pins[pin]);
}
static void delay(void *user,uint32_t us) { (void)user; esp_rom_delay_us(us); }
int ads1299_c6_make_hal(ads1299_c6_context_t *ctx,ads1299_platform_hal_t *hal)
{
    if (!ctx || !hal || !ctx->spi || !ctx->timeout_ticks ||
        ctx->timeout_ticks==portMAX_DELAY || ctx->fault || ctx->in_flight) return -1;
    for(unsigned i=0;i<5;++i) {
        /* IDF validity macro shifts by pin number: bound it before evaluating. */
        if (ctx->pins[i]<0 || ctx->pins[i]>=GPIO_NUM_MAX ||
            !GPIO_IS_VALID_GPIO(ctx->pins[i]) ||
            (i<4 && !GPIO_IS_VALID_OUTPUT_GPIO(ctx->pins[i]))) return -1;
        for(unsigned j=0;j<i;++j) if (ctx->pins[i]==ctx->pins[j]) return -1;
    }
    ads1299_platform_hal_t result={ctx,transfer,write_pin,read_pin,delay};
    *hal=result; return 0;
}
