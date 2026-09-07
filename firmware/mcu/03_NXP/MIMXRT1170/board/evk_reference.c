/* Original thin EVK-derived reference binding; not verified EVKB wiring.
 * Signal mappings: NXP-authored Zephyr mimxrt1170_evk.dtsi; SDK 2.16 IOMUX.
 * Read evk_reference.md before connecting hardware. */
#include "ads1299_mcuxpresso_init.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"

int board_ads1299_hal(ads1299_platform_hal_t *hal) {
    static ads1299_mcuxpresso_ctx_t ctx = {
        .lpspi = LPSPI1,
        .cs = {GPIO9, 28u},
        .reset = {GPIO9, 5u},
        .pwdn = {GPIO9, 4u},
        .start = {GPIO9, 6u},
        .drdy = {GPIO11, 13u},
    };
    if (!hal) return -1;
    /* Keep boot/debugger core clocks; query instead of assuming 1 GHz.
     * SDK 2.16 SystemCoreClockUpdate() is empty on this device. */
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M7);
    if (SystemCoreClock == 0u) return -1;
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_EnableClock(kCLOCK_Gpio);
    const clock_root_config_t spi_root = {
        .clockOff = false,
        .mux = kCLOCK_LPSPI1_ClockRoot_MuxOscRc48MDiv2,
        .div = 1u,
    };
    CLOCK_SetRootClock(kCLOCK_Root_Lpspi1, &spi_root);
    /* Set safe GPIO latches/directions BEFORE switching the pads to GPIO. */
    if (ads1299_mcuxpresso_init(hal, &ctx,
            CLOCK_GetRootClockFreq(kCLOCK_Root_Lpspi1), 1000000u) != 0) return -1;
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_29_GPIO9_IO28, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_06_GPIO9_IO05, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_05_GPIO9_IO04, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_07_GPIO9_IO06, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_DISP_B2_12_GPIO11_IO13, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_28_LPSPI1_SCK, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_30_LPSPI1_SOUT, 0u);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_31_LPSPI1_SIN, 1u);
    return 0;
}
