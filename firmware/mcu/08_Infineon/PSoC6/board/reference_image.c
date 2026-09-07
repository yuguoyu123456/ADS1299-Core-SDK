/* Original CM4 cold-start reference. Uses official CM0P_SLEEP + BSP startup.
 * MCU pin choices are provisional; board header routing is not bench-verified. */
#include "ads1299_psoc6_hal.h"
#include "cy_sysclk.h"
#include "cy_syslib.h"
static ads1299_psoc6_context_t context;
static void delay_us(void *user,uint32_t us)
{
    (void)user;
    while(us) {
        uint16_t part=(uint16_t)(us>60000 ? 60000 : us);
        Cy_SysLib_DelayUs(part); us-=part;
    }
}
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (!hal) return -1;
    SystemCoreClockUpdate(); /* refresh PDL delay calibration and peripheral Hz */
    if (cy_PeriClkFreqHz<8000000U) return -1;
    uint32_t divider=(cy_PeriClkFreqHz+7999999U)/8000000U;
    if (divider>65536U) return -1;
    /* Cold-start exclusive divider 0 and SCB3. Never share this clock resource. */
    if (Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_16_BIT,0,divider-1)!=CY_SYSCLK_SUCCESS ||
        Cy_SysClk_PeriphAssignDivider(PCLK_SCB3_CLOCK,CY_SYSCLK_DIV_16_BIT,0)!=CY_SYSCLK_SUCCESS ||
        Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_16_BIT,0)!=CY_SYSCLK_SUCCESS) return -1;
    NVIC_DisableIRQ(scb_3_interrupt_IRQn);
    Cy_GPIO_Pin_FastInit(GPIO_PRT6,0,CY_GPIO_DM_STRONG_IN_OFF,0,P6_0_SCB3_SPI_MOSI);
    Cy_GPIO_Pin_FastInit(GPIO_PRT6,1,CY_GPIO_DM_HIGHZ,0,P6_1_SCB3_SPI_MISO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT6,2,CY_GPIO_DM_STRONG_IN_OFF,0,P6_2_SCB3_SPI_CLK);
    Cy_GPIO_Pin_FastInit(GPIO_PRT6,3,CY_GPIO_DM_STRONG_IN_OFF,1,HSIOM_SEL_GPIO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT7,0,CY_GPIO_DM_STRONG_IN_OFF,1,HSIOM_SEL_GPIO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT7,1,CY_GPIO_DM_STRONG_IN_OFF,1,HSIOM_SEL_GPIO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT7,2,CY_GPIO_DM_STRONG_IN_OFF,0,HSIOM_SEL_GPIO);
    Cy_GPIO_Pin_FastInit(GPIO_PRT7,3,CY_GPIO_DM_HIGHZ,0,HSIOM_SEL_GPIO);
    context.spi=SCB3;
    context.gpio[0]=GPIO_PRT6; context.pins[0]=3; /* CS */
    for(unsigned i=1;i<5;++i) { context.gpio[i]=GPIO_PRT7; context.pins[i]=i-1; }
    context.poll_limit=100000; context.delay_us=delay_us;
    const cy_stc_scb_spi_config_t cfg={
        .spiMode=CY_SCB_SPI_MASTER,.subMode=CY_SCB_SPI_MOTOROLA,
        .sclkMode=CY_SCB_SPI_CPHA1_CPOL0,.oversample=8,
        .rxDataWidth=8,.txDataWidth=8,.enableMsbFirst=true
    };
    return ads1299_psoc6_make_hal(&context,&cfg,hal);
}
