#include "ads1299_g4_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
volatile uint32_t test_mode;
const uint32_t test_addresses[]={(uint32_t)&SPI1->SR,(uint32_t)&SPI1->DR,(uint32_t)&SPI1->CR1};
const uint32_t test_flags[]={SPI_SR_RXNE,SPI_SR_TXE,SPI_SR_BSY,SPI_SR_OVR,SPI_SR_MODF,SPI_SR_FRE,SPI_CR1_SPE};
static ads1299_g4_context_t ctx;
static ads1299_platform_hal_t hal;
static void delay(void *user,uint32_t us) { (void)user; (void)us; }
static int prepare(void)
{
    SPI1->CR1=0; ctx.spi=SPI1; ctx.source_hz=16000000; ctx.spi_hz=1000000;
    ctx.poll_limit=10; ctx.delay_us=delay; ctx.fault=0;
    for(unsigned i=0;i<5;++i) { ctx.gpio[i]=GPIOC; ctx.pins[i]=(uint16_t)(1U<<i); }
    return ads1299_g4_make_hal(&ctx,&hal);
}
int test_main(void)
{
    uint8_t tx[3]={0,0x80,0xff},rx[3]={0};
    CHECK(prepare()==0);
    CHECK((SPI1->CR1&SPI_CR1_BR)==(3U<<SPI_CR1_BR_Pos));
    CHECK((SPI1->CR1&(SPI_CR1_CPHA|SPI_CR1_CPOL|SPI_CR1_LSBFIRST))==SPI_CR1_CPHA);
    CHECK((SPI1->CR2&SPI_CR2_DS)==(7U<<SPI_CR2_DS_Pos));
    CHECK(hal.spi_transfer(&ctx,tx,rx,3)==0);
    for(unsigned i=0;i<3;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    CHECK(hal.spi_transfer(&ctx,0,rx,3)==0 && rx[0]==0x5a);
    CHECK(hal.spi_transfer(&ctx,tx,0,3)==0);
    CHECK(hal.spi_transfer(&ctx,0,0,1)!=0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,0)!=0);
    for(unsigned mode=1;mode<=7;++mode) {
        test_mode=0; CHECK(prepare()==0); test_mode=mode;
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0 && ctx.fault);
        CHECK(!(SPI1->CR1&SPI_CR1_SPE));
        CHECK(hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
    }
    test_mode=0; CHECK(prepare()==0);
    CHECK(ads1299_g4_make_hal(&ctx,&hal)!=0); /* active peripheral */
    SPI1->CR1=0; ctx.spi_hz=1100000;
    CHECK(ads1299_g4_make_hal(&ctx,&hal)==0);
    CHECK((SPI1->CR1&SPI_CR1_BR)==(3U<<SPI_CR1_BR_Pos));
    SPI1->CR1=0; ctx.spi_hz=1; CHECK(ads1299_g4_make_hal(&ctx,&hal)!=0);
    ctx.spi_hz=1000000; ctx.pins[4]=ctx.pins[0]; CHECK(ads1299_g4_make_hal(&ctx,&hal)!=0);
    return 0;
}
