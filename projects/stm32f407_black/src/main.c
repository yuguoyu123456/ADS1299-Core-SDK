#include "main.h"
#include "ads1299.h"
#include "ads1299_port_stm32_hal.h"
#include "ads1299_packet.h"

#include <stdio.h>
#include <string.h>

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

static volatile uint32_t g_drdy_pending = 0;
static uint32_t g_sequence = 0;
static uint32_t g_dropped_drdy = 0;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void dwt_init(void);
static void delay_us(uint32_t us);
static uint32_t micros32(void);
static void uart_text(const char *s);
static void print_id(uint8_t id);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    dwt_init();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();

    uart_text("\r\nADS1299-Core STM32F407 reference project\r\n");
    uart_text("SPI1 Mode 1, DRDY EXTI, binary v0 host packets\r\n");

    ads1299_stm32_hal_ctx_t hw = {
        .hspi = &hspi1,
        .cs_port = ADS_CS_GPIO_Port,
        .cs_pin = ADS_CS_Pin,
        .reset_port = ADS_RESET_GPIO_Port,
        .reset_pin = ADS_RESET_Pin,
        .start_port = ADS_START_GPIO_Port,
        .start_pin = ADS_START_Pin,
        .drdy_port = ADS_DRDY_GPIO_Port,
        .drdy_pin = ADS_DRDY_Pin,
        .spi_timeout_ms = 10,
        .delay_us_cb = delay_us,
    };

    ads1299_t ads;
    ads1299_port_t port = ads1299_stm32_hal_make_port(&hw);
    if (ads1299_init(&ads, &port) != ADS1299_OK) Error_Handler();

    if (ads1299_hardware_reset(&ads) != ADS1299_OK) Error_Handler();
    delay_us(1000);
    if (ads1299_sdatac(&ads) != ADS1299_OK) Error_Handler();

    uint8_t id = 0;
    if (ads1299_read_register(&ads, ADS1299_REG_ID, &id) != ADS1299_OK) Error_Handler();
    print_id(id);

    /* First boot profile deliberately uses the internal calibration signal.
       This lets a new board be checked without electrodes. */
    if (ads1299_set_data_rate(&ads, ADS1299_DR_250SPS) != ADS1299_OK) Error_Handler();
    if (ads1299_configure_internal_test(&ads, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        Error_Handler();
    }
    if (ads1299_rdatac(&ads) != ADS1299_OK) Error_Handler();
    if (ads1299_start(&ads) != ADS1299_OK) Error_Handler();

    uart_text("STREAM_BEGIN\r\n");

    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];
    for (;;) {
        if (!g_drdy_pending) continue;

        __disable_irq();
        uint32_t pending = g_drdy_pending;
        g_drdy_pending = 0;
        __enable_irq();
        if (pending > 1u) g_dropped_drdy += pending - 1u;

        if (ads1299_read_frame_continuous(&ads, &frame) != ADS1299_OK) {
            uart_text("ADS1299_FRAME_ERROR\r\n");
            continue;
        }

        const uint8_t flags = g_dropped_drdy ? 0x01u : 0x00u;
        size_t n = ads1299_packet_encode(packet, sizeof(packet), flags,
                                         g_sequence++, micros32(),
                                         frame.status, frame.channel);
        if (n == ADS1299_PACKET_SIZE) {
            (void)HAL_UART_Transmit(&huart2, packet, (uint16_t)n, 10);
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == ADS_DRDY_Pin) {
        if (g_drdy_pending != UINT32_MAX) ++g_drdy_pending;
    }
}

void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(ADS_DRDY_Pin);
}

static void dwt_init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us) {
    const uint32_t start = DWT->CYCCNT;
    const uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000u) * us;
    while ((uint32_t)(DWT->CYCCNT - start) < ticks) { }
}

static uint32_t micros32(void) {
    const uint32_t div = HAL_RCC_GetHCLKFreq() / 1000000u;
    return div ? (DWT->CYCCNT / div) : 0u;
}

static void uart_text(const char *s) {
    if (!s) return;
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)s, (uint16_t)strlen(s), 100);
}

static void print_id(uint8_t id) {
    char line[48];
    (void)snprintf(line, sizeof(line), "ADS1299 ID register: 0x%02X\r\n", id);
    uart_text(line);
}

static void MX_SPI1_Init(void) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE; /* SPI mode 1 */
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 921600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler();
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spi) {
    if (spi->Instance != SPI1) return;
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    g.Mode = GPIO_MODE_AF_PP;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    g.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &g);
}

void HAL_UART_MspInit(UART_HandleTypeDef *uart) {
    if (uart->Instance != USART2) return;
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    g.Mode = GPIO_MODE_AF_PP;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    g.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &g);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOB, ADS_CS_Pin | ADS_RESET_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, ADS_START_Pin, GPIO_PIN_RESET);

    GPIO_InitTypeDef g = {0};
    g.Pin = ADS_CS_Pin | ADS_RESET_Pin | ADS_START_Pin;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &g);

    g.Pin = ADS_DRDY_Pin;
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &g);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void SystemClock_Config(void) {
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL.PLLM = 16;
    osc.PLL.PLLN = 336;
    osc.PLL.PLLP = RCC_PLLP_DIV2;
    osc.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK) Error_Handler();
}

void Error_Handler(void) {
    __disable_irq();
    while (1) { }
}
