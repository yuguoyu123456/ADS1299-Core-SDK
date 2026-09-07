#include "ads1299_u5_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
volatile uint32_t test_mode,test_delays,test_delay_us;
const uint32_t test_addresses[]={(uint32_t)&SPI1->SR,(uint32_t)&SPI1->TXDR,
 (uint32_t)&SPI1->RXDR,(uint32_t)&SPI1->CR1,(uint32_t)&SPI1->CR2,
 (uint32_t)&SPI1->IFCR,(uint32_t)&GPIOC->BSRR};
const uint32_t test_flags[]={SPI_SR_RXP,SPI_SR_TXP,SPI_SR_EOT,SPI_SR_OVR,
 SPI_SR_MODF,SPI_SR_TIFRE,SPI_SR_UDR,SPI_CR1_SPE,SPI_CR1_CSTART};
static ads1299_u5_context_t ctx;
static ads1299_platform_hal_t hal;
static uint8_t large[65536];
static void delay(void *user,uint32_t us)
{
    (void)user; ++test_delays; test_delay_us=us;
    if (test_mode==8) ctx.fault=1;
}
static int prepare(void)
{
    SPI1->CR1=0; ctx.spi=SPI1; ctx.source_hz=4000000; ctx.spi_hz=1000000;
    ctx.poll_limit=10; ctx.delay_us=delay; ctx.fault=0;
    for(unsigned i=0;i<5;++i) { ctx.gpio[i]=GPIOC; ctx.pins[i]=(uint16_t)(1U<<i); }
    return ads1299_u5_make_hal(&ctx,&hal);
}
int test_main(void)
{
    uint8_t tx[3]={0,0x80,0xff},rx[3]={0};
    CHECK(prepare()==0 && ctx.eot_delay_us==2);
    CHECK(hal.spi_transfer(&ctx,tx,rx,3)==0);
    for(unsigned i=0;i<3;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    CHECK(test_delays==1 && test_delay_us==2);
    CHECK(!(SPI1->CR1&SPI_CR1_SPE));
    CHECK(hal.spi_transfer(&ctx,0,rx,3)==0 && rx[0]==0x5a);
    CHECK(hal.spi_transfer(&ctx,tx,0,3)==0);
    CHECK(hal.spi_transfer(&ctx,0,0,1)!=0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,0)!=0);
    for(unsigned mode=1;mode<=8;++mode) {
        test_mode=0; CHECK(prepare()==0); test_mode=mode;
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0 && ctx.fault);
        CHECK(!(SPI1->CR1&SPI_CR1_SPE));
        CHECK(hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0);
    }
    test_mode=0; CHECK(prepare()==0); test_delays=0;
    hal.pin_write(&ctx,ADS1299_PIN_CS,0);
    CHECK(hal.spi_transfer(&ctx,0,large,sizeof large)==0);
    CHECK(test_delays==2 && large[0]==0x5a && large[65535]==0x5a);
    hal.pin_write(&ctx,ADS1299_PIN_CS,1);
    ctx.spi_hz=900000; CHECK(ads1299_u5_make_hal(&ctx,&hal)==0);
    CHECK(ctx.eot_delay_us==4); /* actual 500 kHz, not requested 900 kHz */
    ctx.spi_hz=1; CHECK(ads1299_u5_make_hal(&ctx,&hal)!=0);
    return 0;
}
