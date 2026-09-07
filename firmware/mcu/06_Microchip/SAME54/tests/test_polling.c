/* Original test harness; MMIO is supplied by a simulator, not actual hardware. */
#include "ads1299_same54_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while(0)
volatile uint32_t test_mode;
const uint32_t test_spi_base=(uint32_t)SERCOM6_REGS;
const uint32_t test_port_base=(uint32_t)PORT_REGS;
const uint32_t test_flag_offset=offsetof(sercom_spim_registers_t,SERCOM_INTFLAG);
const uint32_t test_data_offset=offsetof(sercom_spim_registers_t,SERCOM_DATA);
const uint32_t test_status_offset=offsetof(sercom_spim_registers_t,SERCOM_STATUS);
const uint32_t test_sync_offset=offsetof(sercom_spim_registers_t,SERCOM_SYNCBUSY);
const uint32_t test_flags[5]={SERCOM_SPIM_INTFLAG_DRE_Msk,SERCOM_SPIM_INTFLAG_RXC_Msk,
    SERCOM_SPIM_INTFLAG_TXC_Msk,SERCOM_SPIM_INTFLAG_ERROR_Msk,SERCOM_SPIM_STATUS_BUFOVF_Msk};
static ads1299_same54_context_t ctx;
static ads1299_platform_hal_t hal;
static void delay(void *user,uint32_t us) { (void)user; (void)us; }
static int prepare(void)
{
    ctx.spi=SERCOM6_REGS; ctx.source_hz=60000000; ctx.spi_hz=1000000;
    ctx.poll_limit=10; ctx.delay_us=delay; ctx.fault=0;
    for(unsigned i=0;i<5;++i) ctx.pins[i]=i;
    return ads1299_same54_make_hal(&ctx,&hal);
}
int test_main(void)
{
    uint8_t tx[3]={0,0x80,0xff},rx[3]={0};
    test_mode=0; CHECK(prepare()==0);
    CHECK(ctx.spi->SPIM.SERCOM_BAUD==29);
    CHECK(ctx.spi->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_CPHA_Msk);
    CHECK(!(ctx.spi->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_CPOL_Msk));
    CHECK(hal.spi_transfer(&ctx,tx,rx,3)==0);
    for(unsigned i=0;i<3;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    CHECK(hal.spi_transfer(&ctx,0,rx,3)==0 && rx[0]==0x5a);
    CHECK(hal.spi_transfer(&ctx,tx,0,3)==0);
    CHECK(hal.spi_transfer(&ctx,0,0,1)!=0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,0)!=0);
    for(unsigned mode=1;mode<=4;++mode) {
        test_mode=0; CHECK(prepare()==0); test_mode=mode;
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0 && ctx.fault);
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0);
        CHECK(hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
    }
    test_mode=5; CHECK(prepare()!=0 && ctx.fault);
    test_mode=0; CHECK(prepare()==0); ctx.pins[4]=ctx.pins[0];
    CHECK(ads1299_same54_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.spi_hz=1;
    CHECK(ads1299_same54_make_hal(&ctx,&hal)!=0);
    CHECK(prepare()==0); ctx.spi_hz=1100000;
    CHECK(ads1299_same54_make_hal(&ctx,&hal)==0 && ctx.spi->SPIM.SERCOM_BAUD==27);
    return 0;
}
