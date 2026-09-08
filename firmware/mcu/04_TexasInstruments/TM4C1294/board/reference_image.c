/* Original cold-boot TM4C1294 reference. No chip protocol code here. */
#include "ads1299_tm4c_hal.h"
#include "inc/hw_memmap.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
volatile uint32_t ads1299_board_wiring_reviewed;
volatile int ads1299_example_exit_code;
volatile uint32_t ads1299_unexpected_interrupt;
extern int ads1299_example_main(void);
static ads1299_tm4c_context_t context;
int main(void)
{
    ads1299_example_exit_code=ads1299_example_main();
    for(;;) __asm volatile("wfi");
}
/* Unused interrupts must stop visibly, not enter an empty vendor weak handler. */
void ads1299_unexpected_irq(void)
{
    __asm volatile("cpsid i");
    ads1299_unexpected_interrupt=1;
    for(;;) __asm volatile("wfi");
}
#define IRQ(name) void name(void) __attribute__((alias("ads1299_unexpected_irq")))
IRQ(GPIOAIntHandler); IRQ(GPIOBIntHandler); IRQ(GPIOCIntHandler); IRQ(GPIODIntHandler);
IRQ(GPIOEIntHandler); IRQ(GPIOFIntHandler); IRQ(GPIOGIntHandler); IRQ(GPIOHIntHandler);
IRQ(GPIOJIntHandler); IRQ(GPIOKIntHandler); IRQ(GPIOLIntHandler); IRQ(GPIOMIntHandler);
IRQ(GPIONIntHandler); IRQ(GPIOPIntHandler); IRQ(GPIOQIntHandler); IRQ(GPIORIntHandler);
IRQ(GPIOSIntHandler); IRQ(GPIOTIntHandler); IRQ(SysTickIntHandler); IRQ(lwIPEthernetIntHandler);
IRQ(UARTIntHandler); IRQ(UARTIntHandler1); IRQ(UARTIntHandler2); IRQ(UARTIntHandler3);
IRQ(UARTIntHandler4); IRQ(UARTIntHandler5); IRQ(UARTIntHandler6); IRQ(UARTIntHandler7);
IRQ(ToneIntHandler); IRQ(I2CIntHandler);
static void delay_us(void *user,uint32_t us)
{
    (void)user;
    /* SysCtlDelay: three clocks/iteration; 6*3 >= 16 clocks per us at PIOSC16. */
    while(us--) SysCtlDelay(6);
}
static int enable(uint32_t peripheral)
{
    SysCtlPeripheralEnable(peripheral);
    uint32_t remaining=100000;
    while(!SysCtlPeripheralReady(peripheral)) if(!--remaining) return -1;
    return 0;
}
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if(!hal || ads1299_board_wiring_reviewed!=1) return -1;
    uint32_t hz=SysCtlClockFreqSet(SYSCTL_OSC_INT|SYSCTL_USE_OSC,16000000);
    if(hz!=16000000 || enable(SYSCTL_PERIPH_GPIOA) || enable(SYSCTL_PERIPH_GPIOL) || enable(SYSCTL_PERIPH_SSI0)) return -1;
    context=(ads1299_tm4c_context_t){.ssi_base=SSI0_BASE,
        .gpio_base={GPIO_PORTA_AHB_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE,GPIO_PORTL_BASE},
        .pins={GPIO_PIN_3,GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3},
        .source_hz=hz,.spi_hz=1000000,.poll_limit=100000,.delay_us=delay_us};
    for(unsigned i=0;i<4;++i) {
        GPIOPinWrite(context.gpio_base[i],context.pins[i],i==ADS1299_PIN_START ? 0 : context.pins[i]);
        GPIOPinTypeGPIOOutput(context.gpio_base[i],context.pins[i]);
    }
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE,GPIO_PIN_3);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0); /* TM4C129: TX, NOT TM4C123 RX. */
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
    GPIOPinTypeSSI(GPIO_PORTA_AHB_BASE,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5);
    return ads1299_tm4c_make_hal(&context,hal);
}
