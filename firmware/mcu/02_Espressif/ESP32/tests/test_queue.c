/* Mock API behavior tests; actual IDF build is separate evidence. */
#include "ads1299_esp32_hal.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while(0)
static ads1299_esp32_context_t ctx;
static ads1299_platform_hal_t hal;
static spi_transaction_t *queued;
static int mode,queue_calls,pin_calls,fixture_error;
void *memcpy(void *d,const void *s,size_t n) { uint8_t *out=d; const uint8_t *in=s; while(n--) *out++=*in++; return d; }
void *memset(void *d,int c,size_t n) { uint8_t *out=d; while(n--) *out++=(uint8_t)c; return d; }
int gpio_set_level(gpio_num_t pin,uint32_t level) { (void)pin; (void)level; ++pin_calls; return mode==4 ? -1 : 0; }
int gpio_get_level(gpio_num_t pin) { (void)pin; return 0; }
void esp_rom_delay_us(uint32_t us) { (void)us; }
int spi_device_queue_trans(spi_device_handle_t spi,spi_transaction_t *t,TickType_t ticks)
{
    (void)spi; ++queue_calls;
    if (t!=&ctx.transaction || t->tx_buffer!=ctx.tx || t->rx_buffer!=ctx.rx ||
        !ticks || ticks==portMAX_DELAY || t->length>256) fixture_error=1;
    if (mode==1) return -1;
    queued=t; return 0;
}
int spi_device_get_trans_result(spi_device_handle_t spi,spi_transaction_t **done,TickType_t ticks)
{
    (void)spi; (void)ticks;
    if (mode==2) return -1;
    for(size_t i=0;i<queued->length/8;++i) ((uint8_t *)queued->rx_buffer)[i]=((const uint8_t *)queued->tx_buffer)[i]^0x5a;
    *done=mode==3 ? NULL : queued; return 0;
}
static int prepare(void)
{
    memset(&ctx,0,sizeof ctx); ctx.spi=(void *)1; ctx.timeout_ticks=100;
    for(unsigned i=0;i<5;++i) ctx.pins[i]=(int)i;
    mode=queue_calls=pin_calls=fixture_error=0; queued=NULL;
    return ads1299_esp32_make_hal(&ctx,&hal);
}
int test_main(void)
{
    uint8_t tx[65],rx[65];
    for(unsigned i=0;i<65;++i) tx[i]=(uint8_t)i;
    CHECK(prepare()==0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,65)==0 && queue_calls==3 && pin_calls==0);
    for(unsigned i=0;i<65;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    CHECK(!fixture_error && !ctx.in_flight);
    CHECK(hal.spi_transfer(&ctx,NULL,rx,33)==0 && rx[32]==0x5a);
    CHECK(hal.spi_transfer(&ctx,tx,NULL,31)==0);
    CHECK(hal.spi_transfer(&ctx,NULL,NULL,1)!=0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,0)!=0);
    for(int m=1;m<=3;++m) {
        CHECK(prepare()==0); mode=m; memset(rx,0xa5,sizeof rx);
        CHECK(hal.spi_transfer(&ctx,tx,rx,33)!=0 && ctx.fault);
        CHECK(ctx.in_flight==(m!=1));
        CHECK(rx[0]==0xa5 && rx[32]==0xa5);
        int calls=queue_calls;
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0 && queue_calls==calls);
        CHECK(hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
        if (m==2) { /* delayed driver completion still owns valid storage */
            CHECK(queued==&ctx.transaction && queued->tx_buffer==ctx.tx);
            mode=0; spi_transaction_t *done=NULL;
            CHECK(spi_device_get_trans_result(ctx.spi,&done,1)==0 && done==&ctx.transaction);
            CHECK(rx[0]==0xa5 && ctx.rx[0]==(uint8_t)(tx[0]^0x5a));
        }
    }
    CHECK(prepare()==0); mode=4; hal.pin_write(&ctx,ADS1299_PIN_CS,1);
    CHECK(ctx.fault && hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
    CHECK(prepare()==0); ctx.pins[4]=ctx.pins[0]; CHECK(ads1299_esp32_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.timeout_ticks=portMAX_DELAY; CHECK(ads1299_esp32_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.pins[4]=100; CHECK(ads1299_esp32_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.pins[4]=-1; CHECK(ads1299_esp32_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.pins[0]=34; CHECK(ads1299_esp32_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.pins[4]=34; CHECK(ads1299_esp32_make_hal(&ctx,&hal)==0);
    return 0;
}
