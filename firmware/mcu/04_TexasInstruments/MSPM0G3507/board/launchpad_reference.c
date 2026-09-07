/* Original hardware-only binding. Cold-reset standalone reference; not a
 * drop-in initializer for an application already using GPIO/SPI/clocks. */
#include "ads1299_mspm0_hal.h"
#include <ti/driverlib/dl_common.h>
#include <ti/driverlib/m0p/dl_sysctl.h>

int board_ads1299_hal(ads1299_platform_hal_t *hal) {
    static ads1299_mspm0_ctx_t ctx = {
        .spi = SPI1, .mclk_hz = 32000000u,
        .cs = {GPIOB, DL_GPIO_PIN_6}, .reset = {GPIOA, DL_GPIO_PIN_26},
        .pwdn = {GPIOA, DL_GPIO_PIN_27}, .start = {GPIOA, DL_GPIO_PIN_28},
        .drdy = {GPIOA, DL_GPIO_PIN_25},
    };
    if (!hal) return -1;
    /* Reset clock source is SYSOSC. No PLL or external oscillator required. */
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_SPI_reset(SPI1);
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_SPI_enablePower(SPI1);
    DL_Common_delayCycles(64u);

    DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_6);
    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_26 | DL_GPIO_PIN_27);
    DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_28);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM23); /* PB6 / GPIO CS, not PCS */
    DL_GPIO_initDigitalOutput(IOMUX_PINCM59); /* PA26 RESET */
    DL_GPIO_initDigitalOutput(IOMUX_PINCM60); /* PA27 PWDN */
    DL_GPIO_initDigitalOutput(IOMUX_PINCM3);  /* PA28 START */
    DL_GPIO_initDigitalInput(IOMUX_PINCM55);  /* PA25 DRDY */
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_6);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_26 | DL_GPIO_PIN_27 | DL_GPIO_PIN_28);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM26, IOMUX_PINCM26_PF_SPI1_SCLK);
    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM25, IOMUX_PINCM25_PF_SPI1_PICO);
    DL_GPIO_initPeripheralInputFunction(IOMUX_PINCM24, IOMUX_PINCM24_PF_SPI1_POCI);
    DL_SPI_ClockConfig clock = {
        .clockSel = DL_SPI_CLOCK_BUSCLK, .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1,
    };
    DL_SPI_Config spi = {
        .mode = DL_SPI_MODE_CONTROLLER,
        .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA1,
        .parity = DL_SPI_PARITY_NONE, .dataSize = DL_SPI_DATA_SIZE_8,
        .bitOrder = DL_SPI_BIT_ORDER_MSB_FIRST, .chipSelectPin = DL_SPI_CHIP_SELECT_0,
    };
    DL_SPI_setClockConfig(SPI1, &clock);
    DL_SPI_init(SPI1, &spi);
    /* 32 MHz / (2 * (15 + 1)) = 1 MHz. No hardware PCS pad is muxed. */
    DL_SPI_setBitRateSerialClockDivider(SPI1, 15u);
    DL_SPI_enable(SPI1);
    return ads1299_mspm0_make_hal(hal, &ctx);
}
