/* Original cold-boot Core0 reference binding; see reference_image.md. */
#include "ads1299_lpc55_hal.h"
#include "board_ads1299_config.h"
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
    /* At the reference 12 MHz clock, twelve NOPs plus loop overhead take at
     * least one us. No calibrated exact pulse-width claim; interrupts only
     * extend the delay. Custom clocking should replace this board-local delay. */
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    }
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (!hal) return -1;
#if ADS1299_BOARD_WIRING_REVIEW_REQUIRED
    if (ads1299_board_wiring_reviewed!=1) return -1;
#endif
    /* This binding does not change boot/security policy or start Core1.
     * The reference image intentionally accepts only the reset FRO12 main
     * clock, AHB /1; custom clock trees belong in the board layer. */
    if ((SYSCON->MAINCLKSELA&SYSCON_MAINCLKSELA_SEL_MASK)!=0 ||
        (SYSCON->MAINCLKSELB&SYSCON_MAINCLKSELB_SEL_MASK)!=0 ||
        SYSCON->AHBCLKDIV!=0 || (PMC->PDRUNCFG0&kPDRUNCFG_PD_FRO192M)) return -1;
    SystemCoreClock=ADS1299_BOARD_SPI_SOURCE_HZ;
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    CLOCK_EnableClock(kCLOCK_Iocon);
    GPIO_PortInit(GPIO,0); GPIO_PortInit(GPIO,1);
    context=(ads1299_lpc55_context_t){.spi=SPI7,.gpio=GPIO,
        .ports={ADS1299_BOARD_CS_PORT,ADS1299_BOARD_RESET_PORT,
                ADS1299_BOARD_PWDN_PORT,ADS1299_BOARD_START_PORT,
                ADS1299_BOARD_DRDY_PORT},
        .pins={ADS1299_BOARD_CS_PIN,ADS1299_BOARD_RESET_PIN,
               ADS1299_BOARD_PWDN_PIN,ADS1299_BOARD_START_PIN,
               ADS1299_BOARD_DRDY_PIN},
        .source_hz=ADS1299_BOARD_SPI_SOURCE_HZ,
        .spi_hz=ADS1299_BOARD_SPI_HZ,
        .poll_limit=ADS1299_BOARD_SPI_POLL_LIMIT,.delay_us=delay_us};
    for(unsigned i=0;i<5;++i) {
        gpio_pin_config_t pin={i==ADS1299_PIN_DRDY ? kGPIO_DigitalInput : kGPIO_DigitalOutput,
            i==ADS1299_PIN_START ? 0 : 1};
        /* Latch and direction before attaching the pad to GPIO. */
        GPIO_PinInit(GPIO,context.ports[i],context.pins[i],&pin);
        IOCON_PinMuxSet(IOCON,context.ports[i],context.pins[i],IOCON_PIO_FUNC(0)|IOCON_PIO_DIGIMODE_MASK);
    }
    IOCON_PinMuxSet(IOCON,ADS1299_BOARD_SPI_PAD_PORT,ADS1299_BOARD_SPI_PAD0_PIN,
                    IOCON_PIO_FUNC(ADS1299_BOARD_SPI_IOCON_FUNC)|IOCON_PIO_DIGIMODE_MASK);
    IOCON_PinMuxSet(IOCON,ADS1299_BOARD_SPI_PAD_PORT,ADS1299_BOARD_SPI_PAD1_PIN,
                    IOCON_PIO_FUNC(ADS1299_BOARD_SPI_IOCON_FUNC)|IOCON_PIO_DIGIMODE_MASK);
    IOCON_PinMuxSet(IOCON,ADS1299_BOARD_SPI_PAD_PORT,ADS1299_BOARD_SPI_PAD2_PIN,
                    IOCON_PIO_FUNC(ADS1299_BOARD_SPI_IOCON_FUNC)|IOCON_PIO_DIGIMODE_MASK);
    return ads1299_lpc55_make_hal(&context,hal);
}
