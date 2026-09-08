/* Original hardware-only adapter using external TivaWare APIs/definitions. */
#include "ads1299_tm4c_hal.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
static int fail(ads1299_tm4c_context_t *c)
{ c->fault=1; SSIDisable(c->ssi_base); return -1; }
static int error(ads1299_tm4c_context_t *c)
{ return (SSIIntStatus(c->ssi_base,false)&SSI_RXOR)!=0; }
static int transfer(void *user,const uint8_t *tx,uint8_t *rx,size_t length)
{
    ads1299_tm4c_context_t *c=user;
    if (!c || c->fault || !length || (!tx && !rx)) return -1;
    uint32_t word;
    if (SSIBusy(c->ssi_base) || error(c) || SSIDataGetNonBlocking(c->ssi_base,&word)) return fail(c);
    for(size_t i=0;i<length;++i) {
        uint32_t budget=c->poll_limit;
        while(!SSIDataPutNonBlocking(c->ssi_base,tx ? tx[i] : 0))
            if (error(c) || !--budget) return fail(c);
        budget=c->poll_limit;
        while(!SSIDataGetNonBlocking(c->ssi_base,&word))
            if (error(c) || !--budget) return fail(c);
        if (error(c)) return fail(c);
        if (rx) rx[i]=(uint8_t)word;
    }
    uint32_t budget=c->poll_limit;
    while(SSIBusy(c->ssi_base)) if (error(c) || !--budget) return fail(c);
    if (error(c) || SSIDataGetNonBlocking(c->ssi_base,&word)) return fail(c);
    return 0;
}
static void write_pin(void *user,ads1299_platform_pin_t pin,int level)
{
    ads1299_tm4c_context_t *c=user;
    if (c && (unsigned)pin<ADS1299_PIN_DRDY)
        GPIOPinWrite(c->gpio_base[pin],c->pins[pin],level ? c->pins[pin] : 0);
}
static int read_pin(void *user,ads1299_platform_pin_t pin)
{
    ads1299_tm4c_context_t *c=user;
    if (!c || c->fault || (unsigned)pin>ADS1299_PIN_DRDY) return -1;
    return GPIOPinRead(c->gpio_base[pin],c->pins[pin]) ? 1 : 0;
}
static void delay(void *user,uint32_t us)
{ ads1299_tm4c_context_t *c=user; if(us) c->delay_us(c->delay_user,us); }
static bool gpio_valid(uint32_t base)
{
    const uint32_t bases[]={GPIO_PORTA_AHB_BASE,GPIO_PORTB_AHB_BASE,GPIO_PORTC_AHB_BASE,
        GPIO_PORTD_AHB_BASE,GPIO_PORTE_AHB_BASE,GPIO_PORTF_AHB_BASE,GPIO_PORTG_AHB_BASE,
        GPIO_PORTH_AHB_BASE,GPIO_PORTJ_AHB_BASE,GPIO_PORTK_BASE,GPIO_PORTL_BASE,
        GPIO_PORTM_BASE,GPIO_PORTN_BASE,GPIO_PORTP_BASE,GPIO_PORTQ_BASE};
    for(unsigned i=0;i<sizeof(bases)/sizeof(bases[0]);++i) if(base==bases[i]) return true;
    return false;
}
int ads1299_tm4c_make_hal(ads1299_tm4c_context_t *c,ads1299_platform_hal_t *hal)
{
    if (!c || !hal || c->fault || !c->source_hz || !c->spi_hz || !c->poll_limit || !c->delay_us) return -1;
    if(c->ssi_base!=SSI0_BASE && c->ssi_base!=SSI1_BASE && c->ssi_base!=SSI2_BASE && c->ssi_base!=SSI3_BASE) return -1;
    for(unsigned i=0;i<5;++i) {
        if(!gpio_valid(c->gpio_base[i]) || !c->pins[i] || (c->pins[i]&(c->pins[i]-1))) return -1;
        for(unsigned j=0;j<i;++j)
            if(c->gpio_base[i]==c->gpio_base[j] && c->pins[i]==c->pins[j]) return -1;
    }
    /* Find an exact legal divisor giving the fastest rate at/below the ceiling. */
    uint64_t required=((uint64_t)c->source_hz+c->spi_hz-1)/c->spi_hz;
    if(required<2 || required>254U*256U) return -1;
    uint32_t best=UINT32_MAX,pre=0,scr=0;
    for(uint32_t p=2;p<=254;p+=2) {
        uint32_t n=(uint32_t)((required+p-1)/p);
        if(n && n<=256 && p*n<best) { best=p*n; pre=p; scr=n-1; }
    }
    if(!pre || HWREG(c->ssi_base+SSI_O_CR1)&SSI_CR1_SSE ||
       HWREG(c->ssi_base+SSI_O_DMACTL) || HWREG(c->ssi_base+SSI_O_IM) || SSIBusy(c->ssi_base)) return -1;
    uint32_t word;
    if(error(c) || SSIDataGetNonBlocking(c->ssi_base,&word)) return fail(c);
    write_pin(c,ADS1299_PIN_CS,1); write_pin(c,ADS1299_PIN_RESET,1);
    write_pin(c,ADS1299_PIN_PWDN,1); write_pin(c,ADS1299_PIN_START,0);
    SSIClockSourceSet(c->ssi_base,SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(c->ssi_base,c->source_hz,SSI_FRF_MOTO_MODE_1,SSI_MODE_MASTER,c->source_hz/2,8);
    HWREG(c->ssi_base+SSI_O_CPSR)=pre;
    HWREG(c->ssi_base+SSI_O_CR0)=(HWREG(c->ssi_base+SSI_O_CR0)&~SSI_CR0_SCR_M)|(scr<<SSI_CR0_SCR_S);
    SSIEnable(c->ssi_base);
    *hal=(ads1299_platform_hal_t){c,transfer,write_pin,read_pin,delay};
    return 0;
}
