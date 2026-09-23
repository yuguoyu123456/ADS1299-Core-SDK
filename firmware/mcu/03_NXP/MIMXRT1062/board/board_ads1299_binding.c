#include "board_ads1299_binding.h"
#include "board_ads1299_config.h"
#include "ads1299_mcuxpresso_init.h"
#include "clock_config.h"
#include "fsl_iomuxc.h"

int board_ads1299_make_hal(ads1299_platform_hal_t *hal)
{
    static ads1299_rt1062_ctx_t ctx = {
        .lpspi = BOARD_ADS1299_LPSPI,
        .cs = {BOARD_ADS1299_CS_GPIO, BOARD_ADS1299_CS_PIN},
        .reset = {BOARD_ADS1299_RESET_GPIO, BOARD_ADS1299_RESET_PIN},
        .pwdn = {BOARD_ADS1299_PWDN_GPIO, BOARD_ADS1299_PWDN_PIN},
        .start = {BOARD_ADS1299_START_GPIO, BOARD_ADS1299_START_PIN},
        .drdy = {BOARD_ADS1299_DRDY_GPIO, BOARD_ADS1299_DRDY_PIN},
    };
    uint32_t lpspi_clock_hz;

    if (hal == 0) {
        return -1;
    }

    BOARD_BootClockRUN();
    SystemCoreClock = CLOCK_GetFreq(kCLOCK_CpuClk);
    lpspi_clock_hz = CLOCK_GetClockRootFreq(kCLOCK_LpspiClkRoot);
    if ((SystemCoreClock == 0u) || (lpspi_clock_hz == 0u) ||
        (BOARD_ADS1299_SPI_BAUD_HZ == 0u)) {
        return -2;
    }

    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio3);

    if (ads1299_rt1062_init(hal, &ctx, lpspi_clock_hz,
                            BOARD_ADS1299_SPI_BAUD_HZ) != 0) {
        return -3;
    }

    /* Reference EVKB routing. Keep custom-board changes in board/config only. */
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
