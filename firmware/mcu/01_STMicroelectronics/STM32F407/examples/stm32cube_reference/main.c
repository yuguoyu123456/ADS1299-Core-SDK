#include "ads1299.h"
#include "ads1299_spi.h"
#include "ads1299_stm32cube_hal.h"
#include "stm32f4xx_hal.h"

static SPI_HandleTypeDef spi1;

static void gpio_init(void) {
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* Disable the on-board LIS3DSH, which shares SPI1 on MB997. */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &gpio);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_11 | GPIO_PIN_12,
                      GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio);
}

static int spi_init(void) {
    spi1.Instance = SPI1;
    spi1.Init.Mode = SPI_MODE_MASTER;
    spi1.Init.Direction = SPI_DIRECTION_2LINES;
    spi1.Init.DataSize = SPI_DATASIZE_8BIT;
    spi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    spi1.Init.NSS = SPI_NSS_SOFT;
    spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi1.Init.TIMode = SPI_TIMODE_DISABLE;
    spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi1.Init.CRCPolynomial = 7u;
    return HAL_SPI_Init(&spi1) == HAL_OK ? 0 : -1;
}

int main(void) {
    ads1299_platform_hal_t platform_hal;
    ads1299_mcu_port_t mcu_port;
    ads1299_port_t core_port;
    ads1299_stm32cube_context_t cube = {
        .spi = &spi1,
        .gpio = {
            [ADS1299_PIN_CS] = {GPIOB, GPIO_PIN_0},
            [ADS1299_PIN_RESET] = {GPIOB, GPIO_PIN_11},
            [ADS1299_PIN_PWDN] = {GPIOB, GPIO_PIN_12},
            [ADS1299_PIN_START] = {GPIOB, GPIO_PIN_13},
            [ADS1299_PIN_DRDY] = {GPIOB, GPIO_PIN_1},
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
            ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) return 11;
    }
}
