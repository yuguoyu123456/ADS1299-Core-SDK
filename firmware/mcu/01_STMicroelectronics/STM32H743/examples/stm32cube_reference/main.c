#include "ads1299.h"
#include "ads1299_spi.h"
#include "ads1299_stm32cube_hal.h"
#include "stm32h7xx_hal.h"

static SPI_HandleTypeDef spi1;

static void gpio_init(void) {
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

    gpio.Pin = GPIO_PIN_14;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &gpio);

    gpio.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOG, &gpio);

    gpio.Pin = GPIO_PIN_9 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOE, &gpio);

    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &gpio);
}

static int spi_init(void) {
    spi1.Instance = SPI1;
    spi1.Init.Mode = SPI_MODE_MASTER;
    spi1.Init.Direction = SPI_DIRECTION_2LINES;
    spi1.Init.DataSize = SPI_DATASIZE_8BIT;
    spi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    spi1.Init.NSS = SPI_NSS_SOFT;
    spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi1.Init.TIMode = SPI_TIMODE_DISABLE;
    spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi1.Init.CRCPolynomial = 7u;
    spi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    spi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    spi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    spi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    spi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    spi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    spi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    spi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    spi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
    spi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    return HAL_SPI_Init(&spi1) == HAL_OK ? 0 : -1;
}

int main(void) {
    ads1299_platform_hal_t platform_hal;
    ads1299_mcu_port_t mcu_port;
    ads1299_port_t core_port;
    ads1299_stm32cube_context_t cube = {
        .spi = &spi1,
        .gpio = {
            [ADS1299_PIN_CS] = {GPIOD, GPIO_PIN_14},
            [ADS1299_PIN_RESET] = {GPIOG, GPIO_PIN_12},
            [ADS1299_PIN_PWDN] = {GPIOE, GPIO_PIN_9},
            [ADS1299_PIN_START] = {GPIOE, GPIO_PIN_11},
            [ADS1299_PIN_DRDY] = {GPIOF, GPIO_PIN_3},
        },
        .spi_timeout_ms = 10u,
    };
    ads1299_t device;
    ads1299_device_id_t identity;
    ads1299_frame_t frame;

    HAL_Init();
    SystemCoreClockUpdate();
    gpio_init();
    if (spi_init() != 0) return 1;
    if (ads1299_stm32cube_make_hal(&platform_hal, &cube) != 0) return 2;
    if (ads1299_mcu_port_init(&mcu_port, &platform_hal) != 0) return 3;
    core_port = ads1299_mcu_make_port(&mcu_port);
    if (ads1299_init(&device, &core_port) != ADS1299_OK) return 4;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 5;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 6;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 7;
    if (ads1299_configure_internal_test(&device, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return 8;
    if (ads1299_rdatac(&device) != ADS1299_OK) return 9;
    if (ads1299_start(&device) != ADS1299_OK) return 10;

    for (;;) {
        if (core_port.drdy_read(core_port.user) == 0 &&
            ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) {
            return 11;
        }
    }
}
