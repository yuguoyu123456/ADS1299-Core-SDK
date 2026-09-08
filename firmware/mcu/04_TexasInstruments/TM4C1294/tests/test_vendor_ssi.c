#include "ads1299_tm4c_hal.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#define CHECK(x) do { if(!(x)) return __LINE__; } while(0)
const uint32_t model_layout[]={SSI0_BASE,GPIO_PORTL_BASE,SSI_O_SR,SSI_O_DR,SSI_O_RIS,
    SSI_SR_TNF,SSI_SR_RNE,SSI_SR_BSY,SSI_RXOR};
static void delay_us(void *u,uint32_t us) { (void)u; (void)us; }
int test_main(unsigned scenario)
{
    ads1299_tm4c_context_t c={.ssi_base=SSI0_BASE,
        .gpio_base={GPIO_PORTL_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE},
        .pins={1,2,4,8,16},.source_hz=16000000,.spi_hz=1100000,.poll_limit=10,.delay_us=delay_us};
    ads1299_platform_hal_t h;
    CHECK(ads1299_tm4c_make_hal(&c,&h)==0);
    uint32_t cr=HWREG(SSI0_BASE+SSI_O_CR0);
    CHECK((cr&(SSI_CR0_SPO|SSI_CR0_SPH|SSI_CR0_DSS_M))==(SSI_CR0_SPH|7));
    CHECK(HWREG(SSI0_BASE+SSI_O_CPSR)*(((cr&SSI_CR0_SCR_M)>>SSI_CR0_SCR_S)+1)==16);
    uint8_t tx[65],rx[65]; for(unsigned i=0;i<65;++i) tx[i]=(uint8_t)i;
    h.pin_write(&c,ADS1299_PIN_CS,0);
    int status=h.spi_transfer(&c,tx,rx,65);
    if(scenario) {
        CHECK(status<0 && c.fault && !(HWREG(SSI0_BASE+SSI_O_CR1)&SSI_CR1_SSE));
        CHECK(h.spi_transfer(&c,tx,rx,1)<0 && h.pin_read(&c,ADS1299_PIN_DRDY)<0);
    } else {
        CHECK(status==0 && !c.fault);
        for(unsigned i=0;i<65;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0x5a));
        CHECK(h.spi_transfer(&c,0,rx,1)==0 && rx[0]==0x5a);
        CHECK(h.spi_transfer(&c,tx,0,1)==0);
        CHECK(h.spi_transfer(&c,0,0,1)<0 && h.spi_transfer(&c,tx,rx,0)<0);
    }
    h.pin_write(&c,ADS1299_PIN_CS,1);
    CHECK(HWREG(GPIO_PORTL_BASE+4)==1);
    c.fault=0; c.pins[1]=1; CHECK(ads1299_tm4c_make_hal(&c,&h)<0);
    c.pins[1]=2; c.poll_limit=0; CHECK(ads1299_tm4c_make_hal(&c,&h)<0);
    return 0;
}
