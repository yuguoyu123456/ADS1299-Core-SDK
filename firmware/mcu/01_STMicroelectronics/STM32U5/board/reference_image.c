/* Original reset-MSI4 reference. Requires TrustZone disabled by board setup.
 * Never modifies security option bytes. Not a qualified physical pin map. */
#include "ads1299_u5_hal.h"
static ads1299_u5_context_t context;
static void delay_us(void *user,uint32_t us)
{
    (void)user;
    while(us && !context.fault) {
        uint32_t part=us>1000 ? 1000 : us;
        uint32_t start=DWT->CYCCNT,limit=1000000;
        while((uint32_t)(DWT->CYCCNT-start)<part*4U)
            if (!--limit) { context.fault=1; return; }
        us-=part;
    }
}
static void mode(GPIO_TypeDef *port,unsigned pin,unsigned value)
{
    port->MODER=(port->MODER&~(3UL<<(2*pin)))|(value<<(2*pin));
    port->PUPDR &= ~(3UL<<(2*pin)); port->OTYPER &= ~(1UL<<pin);
    port->OSPEEDR=(port->OSPEEDR&~(3UL<<(2*pin)))|(1UL<<(2*pin));
}
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    /* Access itself presumes a TrustZoneDisabled boot, not a secure loader. */
    if (FLASH->OPTR&FLASH_OPTR_TZEN) return -1;
    SystemCoreClockUpdate();
    if (SystemCoreClock!=4000000 || (RCC->CFGR1&RCC_CFGR1_SWS) ||
        (RCC->CFGR2&RCC_CFGR2_HPRE)) return -1;
    RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN|RCC_AHB2ENR1_GPIOCEN;
    (void)RCC->AHB2ENR1;
    RCC->CCIPR1=(RCC->CCIPR1&~RCC_CCIPR1_SPI1SEL)|RCC_CCIPR1_SPI1SEL_0;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; (void)RCC->APB2ENR;
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
    NVIC_DisableIRQ(SPI1_IRQn);
    for(unsigned pin=5;pin<=7;++pin) {
        GPIOA->AFR[0]=(GPIOA->AFR[0]&~(15UL<<(pin*4)))|(5UL<<(pin*4));
        mode(GPIOA,pin,2);
    }
    GPIOC->BSRR=7U|(1UL<<19);
    for(unsigned pin=0;pin<5;++pin) mode(GPIOC,pin,pin==4 ? 0 : 1);
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT=0; DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    for(volatile unsigned i=0;i<32;++i) __NOP();
    if (!DWT->CYCCNT) return -1;
    context.spi=SPI1; context.source_hz=4000000; context.spi_hz=1000000;
    context.poll_limit=100000; context.delay_us=delay_us;
    for(unsigned i=0;i<5;++i) { context.gpio[i]=GPIOC; context.pins[i]=(uint16_t)(1U<<i); }
    return ads1299_u5_make_hal(&context,hal);
}
