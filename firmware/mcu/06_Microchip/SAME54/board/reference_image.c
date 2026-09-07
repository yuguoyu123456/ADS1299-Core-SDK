/* Original cold-start reference. MCU control-pin choices are provisional;
 * verify your board routing before wiring. No vendor startup is copied here. */
#include "ads1299_same54_hal.h"
#include "peripheral/clock/plib_clock.h"
#include "peripheral/nvmctrl/plib_nvmctrl.h"
#include "peripheral/port/plib_port.h"

static ads1299_same54_context_t context;

static void delay_us(void *user, uint32_t us)
{
    ads1299_same54_context_t *ctx=user;
    while (us && !ctx->fault) {
        uint32_t chunk=us>1000 ? 1000 : us;
        uint32_t start=DWT->CYCCNT, budget=1000000;
        while ((uint32_t)(DWT->CYCCNT-start)<chunk*120U) {
            if (--budget==0) { ctx->fault=1; return; }
        }
        us-=chunk;
    }
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (!hal) return -1;
    /* External, unchanged Harmony clock profile: CPU 120 MHz, SERCOM6 60 MHz.
     * Vendor oscillator-start waits are not covered by adapter poll bounds. */
    NVMCTRL_Initialize();
    PORT_Initialize(); /* PC04/05/07 = SERCOM6 PAD0/1/3, mux C */
    CLOCK_Initialize();
    NVIC_DisableIRQ(SERCOM6_0_IRQn); NVIC_DisableIRQ(SERCOM6_1_IRQn);
    NVIC_DisableIRQ(SERCOM6_2_IRQn); NVIC_DisableIRQ(SERCOM6_OTHER_IRQn);
    /* CS PC06, RESET PB04, PWDN PB05, START PB06, DRDY PA02.
     * Set output latch before direction; disconnect conflicting board devices. */
    const uint32_t ids[5]={70,36,37,38,2};
    for (unsigned i=0;i<5;++i) {
        uint32_t id=ids[i], group=id/32U, bit=id%32U, mask=1UL<<bit;
        context.pins[i]=id;
        PORT_REGS->GROUP[group].PORT_PINCFG[bit]=i==4 ? PORT_PINCFG_INEN_Msk : 0;
        if (i==4) PORT_REGS->GROUP[group].PORT_DIRCLR=mask;
        else {
            if (i==3) PORT_REGS->GROUP[group].PORT_OUTCLR=mask;
            else PORT_REGS->GROUP[group].PORT_OUTSET=mask;
            PORT_REGS->GROUP[group].PORT_DIRSET=mask;
        }
    }
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB(); __ISB();
    uint32_t before=DWT->CYCCNT;
    for (volatile unsigned i=0;i<100;++i) { __NOP(); }
    if (DWT->CYCCNT==before) return -1;
    context.spi=SERCOM6_REGS; context.source_hz=60000000;
    context.spi_hz=1000000; context.poll_limit=100000;
    context.delay_user=&context; context.delay_us=delay_us;
    return ads1299_same54_make_hal(&context,hal);
}
