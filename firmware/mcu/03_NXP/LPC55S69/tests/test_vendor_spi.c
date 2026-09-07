/* Real fsl_spi.c is linked; only Flexcomm clock/reset initialization is doubled. */
#include "ads1299_lpc55_hal.h"
#include "fsl_flexcomm.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while(0)
const uint32_t model_layout[]={SPI7_BASE,GPIO_BASE,
    offsetof(SPI_Type,FIFOCFG),offsetof(SPI_Type,FIFOSTAT),offsetof(SPI_Type,FIFORD),
    offsetof(SPI_Type,FIFOWR),offsetof(SPI_Type,STAT),SPI_FIFOSTAT_TXNOTFULL_MASK,
    SPI_FIFOSTAT_TXEMPTY_MASK,SPI_FIFOSTAT_RXNOTEMPTY_MASK,SPI_STAT_MSTIDLE_MASK,
    SPI_FIFOSTAT_RXERR_MASK,SPI_FIFOCFG_SIZE(1),SPI_FIFOWR_EOT_MASK};
status_t FLEXCOMM_Init(void *base,FLEXCOMM_PERIPH_T peripheral)
{ (void)base; (void)peripheral; return kStatus_Success; }
static void delay_us(void *u,uint32_t us) { (void)u; (void)us; }
int test_main(unsigned scenario)
{
    ads1299_lpc55_context_t c={.spi=SPI7,.gpio=GPIO,.ports={0,0,0,0,0},.pins={0,1,2,3,4},
        .source_hz=12000000,.spi_hz=1000000,.poll_limit=100,.delay_us=delay_us};
    ads1299_platform_hal_t h;
    CHECK(ads1299_lpc55_make_hal(&c,&h)==0);
    CHECK((SPI7->CFG&(SPI_CFG_CPOL_MASK|SPI_CFG_CPHA_MASK|SPI_CFG_LSBF_MASK))==SPI_CFG_CPHA_MASK);
    uint8_t tx[65],rx[65]; for(unsigned i=0;i<65;++i) tx[i]=(uint8_t)i;
    h.pin_write(&c,ADS1299_PIN_CS,0);
    int status=h.spi_transfer(&c,tx,rx,sizeof(tx));
    if (!scenario) {
        CHECK(status==0 && !c.fault && GPIO->B[0][0]==0);
        for(unsigned i=0;i<65;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    } else {
        CHECK(status<0 && c.fault && !(SPI7->CFG&SPI_CFG_ENABLE_MASK));
        CHECK(h.spi_transfer(&c,tx,rx,1)<0);
    }
    h.pin_write(&c,ADS1299_PIN_CS,1); CHECK(GPIO->B[0][0]==1);
    return 0;
}
