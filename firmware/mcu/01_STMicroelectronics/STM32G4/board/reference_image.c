/* Original cold-reset HSI16 profile; not a board-header qualification. */
#include "ads1299_g4_hal.h"
static ads1299_g4_context_t context;
static void delay_us(void *user,uint32_t us)
{
    (void)user;
    while (us && !context.fault) {
        uint32_t part=us>1000 ? 1000 : us;
        uint32_t start=DWT->CYCCNT,limit=1000000;
        while ((uint32_t)(DWT->CYCCNT-start)<part*16U) {
            if (!--limit) { context.fault=1; return; }
        }
        us-=part;
    }
}
static void mode(GPIO_TypeDef *port,unsigned pin,unsigned value)
{
    port->MODER=(port->MODER&~(3UL<<(2*pin)))|(value<<(2*pin));
    port->PUPDR &= ~(3UL<<(2*pin));
    port->OTYPER &= ~(1UL<<pin);
    port->OSPEEDR=(port->OSPEEDR&~(3UL<<(2*pin)))|(1UL<<(2*pin));
}
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    SystemCoreClockUpdate();
    /* Reject clock profiles other than reset HSI16 with undivided APB2. */
    if (SystemCoreClock!=16000000U || (RCC->CFGR&(RCC_CFGR_SWS|RCC_CFGR_HPRE|RCC_CFGR_PPRE2))) return -1;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN|RCC_AHB2ENR_GPIOCEN;
    (void)RCC->AHB2ENR;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; (void)RCC->APB2ENR;
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
    NVIC_DisableIRQ(SPI1_IRQn);
    for(unsigned pin=3;pin<=5;++pin) {
        GPIOB->AFR[0]=(GPIOB->AFR[0]&~(15UL<<(pin*4)))|(5UL<<(pin*4));
        mode(GPIOB,pin,2);
    }
    GPIOC->BSRR=7U|(1UL<<(3+16));
    for(unsigned pin=0;pin<5;++pin) mode(GPIOC,pin,pin==4 ? 0 : 1);
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT=0; DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    for(volatile unsigned i=0;i<32;++i) __NOP();
    if (!DWT->CYCCNT) return -1;
    context.spi=SPI1; context.source_hz=16000000; context.spi_hz=1000000;
    context.poll_limit=100000; context.delay_us=delay_us;
    for(unsigned i=0;i<5;++i) { context.gpio[i]=GPIOC; context.pins[i]=(uint16_t)(1U<<i); }
    return ads1299_g4_make_hal(&context,hal);
}
