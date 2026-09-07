/* Executes original adapter and real PDL against modeled peripheral registers. */
#include "ads1299_psoc6_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
volatile uint32_t test_mode;
const uint32_t test_addresses[]={ (uint32_t)&SCB_RX_FIFO_STATUS(SCB3),
    (uint32_t)&SCB_TX_FIFO_STATUS(SCB3),(uint32_t)&SCB_RX_FIFO_RD(SCB3),
    (uint32_t)&SCB_TX_FIFO_WR(SCB3),(uint32_t)&SCB_INTR_RX(SCB3),
    (uint32_t)&SCB_CTRL(SCB3)};
const uint32_t test_flags[]={SCB_TX_FIFO_STATUS_SR_VALID_Msk,
    CY_SCB_SPI_RX_OVERFLOW,CY_SCB_SPI_RX_NO_DATA,SCB_CTRL_ENABLED_Msk};
static ads1299_psoc6_context_t ctx;
static ads1299_platform_hal_t hal;
static void delay(void *user,uint32_t us) { (void)user; (void)us; }
static cy_stc_scb_spi_config_t config={.spiMode=CY_SCB_SPI_MASTER,
    .subMode=CY_SCB_SPI_MOTOROLA,.sclkMode=CY_SCB_SPI_CPHA1_CPOL0,
    .oversample=8,.rxDataWidth=8,.txDataWidth=8,.enableMsbFirst=true};
static int prepare(void)
{
    ctx.spi=SCB3; ctx.poll_limit=10; ctx.delay_us=delay; ctx.fault=0;
    for(unsigned i=0;i<5;++i) { ctx.gpio[i]=GPIO_PRT7; ctx.pins[i]=i; }
    return ads1299_psoc6_make_hal(&ctx,&config,&hal);
}
int test_main(void)
{
    uint8_t tx[3]={0,0x80,0xff},rx[3]={0};
    CHECK(prepare()==0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,3)==0);
    for(unsigned i=0;i<3;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
    CHECK(hal.spi_transfer(&ctx,0,rx,3)==0 && rx[0]==0x5a);
    CHECK(hal.spi_transfer(&ctx,tx,0,3)==0);
    CHECK(hal.spi_transfer(&ctx,0,0,1)!=0);
    CHECK(hal.spi_transfer(&ctx,tx,rx,0)!=0);
    for(unsigned mode=1;mode<=6;++mode) {
        test_mode=0; CHECK(prepare()==0); test_mode=mode;
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0 && ctx.fault);
        CHECK(!(SCB_CTRL(ctx.spi)&SCB_CTRL_ENABLED_Msk));
        CHECK(hal.spi_transfer(&ctx,tx,rx,1)!=0);
        CHECK(hal.pin_read(&ctx,ADS1299_PIN_DRDY)<0);
    }
    test_mode=0; CHECK(prepare()==0); config.sclkMode=CY_SCB_SPI_CPHA0_CPOL0;
    CHECK(ads1299_psoc6_make_hal(&ctx,&config,&hal)!=0);
    config.sclkMode=CY_SCB_SPI_CPHA1_CPOL0; ctx.pins[4]=ctx.pins[0];
    CHECK(ads1299_psoc6_make_hal(&ctx,&config,&hal)!=0);
    return 0;
}
