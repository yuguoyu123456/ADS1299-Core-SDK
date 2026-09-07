/* Thin original EVKB binding; vendor clocks remain in the external SDK.
 * Confirm board/pinmap.md and actual routing before hardware use. */
#include "ads1299_mcuxpresso_init.h"
#include "clock_config.h"
#include "fsl_iomuxc.h"

int board_ads1299_hal(ads1299_platform_hal_t *hal) {
    static ads1299_rt1062_ctx_t ctx = {
        .lpspi = LPSPI1,
        .cs = {GPIO3, 13u}, .reset = {GPIO1, 24u},
        .pwdn = {GPIO1, 9u}, .start = {GPIO1, 10u}, .drdy = {GPIO1, 11u},
    };
    if (!hal) return -1;
    BOARD_BootClockRUN();
    SystemCoreClock = CLOCK_GetFreq(kCLOCK_CpuClk);
    if (SystemCoreClock == 0u) return -1;
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio3);
    /* Safe GPIO levels before mux change. Core owns subsequent reset pulses. */
    if (ads1299_rt1062_init(hal, &ctx,
            CLOCK_GetClockRootFreq(kCLOCK_LpspiClkRoot), 1000000u) != 0) return -1;
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_GPIO3_IO13, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_08_GPIO1_IO24, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_09_GPIO1_IO09, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_10_GPIO1_IO10, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_GPIO1_IO11, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_03_LPSPI1_SDI, 1u);
    return 0;
}
