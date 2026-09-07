/* Original cold-boot Core0 reference binding; see reference_image.md. */
#include "ads1299_lpc55_hal.h"
#include "fsl_iocon.h"
#include "fsl_clock.h"
#include "fsl_power.h"

/* Deliberate interlock: review board wiring before setting to 1 in debugger.
 * Volatile preserves the complete integration path in the linked ELF. */
volatile uint32_t ads1299_board_wiring_reviewed;
static ads1299_lpc55_context_t context;
volatile int ads1299_example_exit_code;
extern void __libc_init_array(void);
extern int main(void);
void ads1299_reference_start(void)
{
    __libc_init_array();
    ads1299_example_exit_code=main();
    for (;;) __WFI();
}

static void delay_us(void *user,uint32_t us)
{
    (void)user;
    /* At 12 MHz, twelve NOPs plus loop overhead take at least one us.
     * No calibrated exact pulse-width claim; interrupts only extend the delay. */
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    }
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (!hal || ads1299_board_wiring_reviewed!=1) return -1;
    /* This binding does not change boot/security policy or start Core1.
     * Accept only the reset FRO12 main clock, AHB /1. */
    if ((SYSCON->MAINCLKSELA&SYSCON_MAINCLKSELA_SEL_MASK)!=0 ||
        (SYSCON->MAINCLKSELB&SYSCON_MAINCLKSELB_SEL_MASK)!=0 ||
        SYSCON->AHBCLKDIV!=0 || (PMC->PDRUNCFG0&kPDRUNCFG_PD_FRO192M)) return -1;
    SystemCoreClock=12000000;
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    CLOCK_EnableClock(kCLOCK_Iocon);
    GPIO_PortInit(GPIO,0); GPIO_PortInit(GPIO,1);
    context=(ads1299_lpc55_context_t){.spi=SPI7,.gpio=GPIO,
        .ports={1,0,0,1,1},.pins={31,26,27,6,7},
        .source_hz=12000000,.spi_hz=1000000,.poll_limit=100000,.delay_us=delay_us};
    for(unsigned i=0;i<5;++i) {
        gpio_pin_config_t pin={i==ADS1299_PIN_DRDY ? kGPIO_DigitalInput : kGPIO_DigitalOutput,
            i==ADS1299_PIN_START ? 0 : 1};
        /* Latch and direction before attaching the pad to GPIO. */
        GPIO_PinInit(GPIO,context.ports[i],context.pins[i],&pin);
        IOCON_PinMuxSet(IOCON,context.ports[i],context.pins[i],IOCON_PIO_FUNC(0)|IOCON_PIO_DIGIMODE_MASK);
    }
    for(unsigned pin=19;pin<=21;++pin)
        IOCON_PinMuxSet(IOCON,0,pin,IOCON_PIO_FUNC(7)|IOCON_PIO_DIGIMODE_MASK);
    return ads1299_lpc55_make_hal(&context,hal);
}
