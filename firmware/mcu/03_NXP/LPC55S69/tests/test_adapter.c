/* Genuine SDK types; SPI API test doubles, not a peripheral simulation. */
#include "ads1299_lpc55_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static SPI_Type spi;
static GPIO_Type gpio;
static unsigned calls,mode,bad,elapsed;
static size_t sizes[4];
static void wait_us(void *u,uint32_t us) { (void)u; elapsed+=us; }
void SPI_MasterGetDefaultConfig(spi_master_config_t *c)
{ unsigned char *p=(unsigned char *)c; for(size_t i=0;i<sizeof(*c);++i) p[i]=0; }
status_t SPI_MasterInit(SPI_Type *b,const spi_master_config_t *c,uint32_t hz)
{
    if(c->phase!=kSPI_ClockPhaseSecondEdge || c->polarity!=kSPI_ClockPolarityActiveHigh ||
       c->direction!=kSPI_MsbFirst || c->dataWidth!=kSPI_Data8Bits || hz!=12000000) bad=1;
    b->CFG=SPI_CFG_ENABLE_MASK;
    return kStatus_Success;
}
status_t SPI_MasterTransferBlocking(SPI_Type *b,spi_transfer_t *x)
{
    if(calls>=4 || !x->txData || !x->rxData || x->dataSize>32 || gpio.B[0][0]!=0) bad=1;
    sizes[calls++]=x->dataSize;
    if(mode==1) return kStatus_Fail;
    if(mode==2) b->STAT=0;
    if(mode==3) b->FIFOSTAT=SPI_FIFOSTAT_RXERR_MASK;
    for(size_t i=0;i<x->dataSize;++i) x->rxData[i]=(uint8_t)(x->txData[i]^0x5a);
    return kStatus_Success;
}
static ads1299_lpc55_context_t fresh(void)
{
    spi.STAT=SPI_STAT_MSTIDLE_MASK; spi.FIFOSTAT=0;
    calls=mode=bad=elapsed=0;
    ads1299_lpc55_context_t c={.spi=&spi,.gpio=&gpio,.ports={0,0,0,0,0},
        .pins={0,1,2,3,4},.source_hz=12000000,.spi_hz=1100000,
        .poll_limit=4,.delay_us=wait_us};
    return c;
}
int test_main(void)
{
    ads1299_platform_hal_t h;
    ads1299_lpc55_context_t c=fresh();
    CHECK(ads1299_lpc55_make_hal(&c,&h)==0 && !bad && spi.DIV==10);
    CHECK(gpio.B[0][0]==1 && gpio.B[0][1]==1 && gpio.B[0][2]==1 && gpio.B[0][3]==0);
    uint8_t tx[65],rx[65]; for(unsigned i=0;i<65;++i) tx[i]=(uint8_t)i;
    h.pin_write(&c,ADS1299_PIN_CS,0);
    CHECK(h.spi_transfer(&c,tx,rx,65)==0 && calls==3 && !bad);
    CHECK(sizes[0]==32 && sizes[1]==32 && sizes[2]==1 && gpio.B[0][0]==0);
    for(unsigned i=0;i<65;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    calls=0; CHECK(h.spi_transfer(&c,0,rx,1)==0 && rx[0]==0x5a);
    calls=0; CHECK(h.spi_transfer(&c,tx,0,1)==0);
    CHECK(h.spi_transfer(&c,0,0,1)<0 && h.spi_transfer(&c,tx,rx,0)<0);
    h.delay_us(&c,10); CHECK(elapsed==10);
    for(unsigned scenario=1;scenario<=3;++scenario) {
        c=fresh(); CHECK(ads1299_lpc55_make_hal(&c,&h)==0);
        h.pin_write(&c,ADS1299_PIN_CS,0); mode=scenario;
        CHECK(h.spi_transfer(&c,tx,rx,1)<0 && c.fault && !(spi.CFG&SPI_CFG_ENABLE_MASK));
        CHECK(h.spi_transfer(&c,tx,rx,1)<0 && calls==1);
        CHECK(h.pin_read(&c,ADS1299_PIN_DRDY)<0);
        h.pin_write(&c,ADS1299_PIN_CS,1); CHECK(gpio.B[0][0]==1);
    }
    c=fresh(); CHECK(ads1299_lpc55_make_hal(&c,&h)==0);
    spi.FIFOSTAT=SPI_FIFOSTAT_RXNOTEMPTY_MASK;
    CHECK(h.spi_transfer(&c,tx,rx,1)<0 && calls==0 && c.fault);
    c=fresh(); c.pins[4]=c.pins[0]; CHECK(ads1299_lpc55_make_hal(&c,&h)<0);
    c=fresh(); c.poll_limit=0; CHECK(ads1299_lpc55_make_hal(&c,&h)<0);
    c=fresh(); c.spi_hz=1; CHECK(ads1299_lpc55_make_hal(&c,&h)<0);
    return 0;
}
