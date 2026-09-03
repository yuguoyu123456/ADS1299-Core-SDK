#include "ads1299.h"
#include "ads1299_packet.h"
#include "ads1299_port_wch.h"
#include "ch32v30x.h"
#include "debug.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ADS_CS_PORT       GPIOB
#define ADS_CS_PIN        GPIO_Pin_6
#define ADS_DRDY_PORT     GPIOB
#define ADS_DRDY_PIN      GPIO_Pin_7
#define ADS_RESET_PORT    GPIOB
#define ADS_RESET_PIN     GPIO_Pin_8
#define ADS_START_PORT    GPIOB
#define ADS_START_PIN     GPIO_Pin_9

#define ADS_SAMPLE_RATE_HZ 250u
#define ADS_SAMPLE_PERIOD_US (1000000u / ADS_SAMPLE_RATE_HZ)

static volatile uint32_t g_drdy_pending = 0u;
static uint32_t g_sequence = 0u;
static uint32_t g_dropped_drdy = 0u;
static uint32_t g_stream_time_us = 0u;

static void board_gpio_init(void);
static void spi1_init(void);
static void uart1_init(void);
static void drdy_exti_init(void);
static void raw_uart_write(const void *data, size_t len);
static void uart_text(const char *text);
static void uart_hex8(const char *prefix, uint8_t value);
static void fatal(const char *reason);

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();

    board_gpio_init();
    uart1_init();
    spi1_init();
    drdy_exti_init();

    uart_text("\r\nADS1299-Core CH32V307 reference project\r\n");
    uart_text("WCH NoneOS SDK | SPI1 Mode 1 | DRDY EXTI | 250 SPS internal test\r\n");

    ads1299_wch_ctx_t hw = {
        .spi = SPI1,
        .cs_port = ADS_CS_PORT,
        .cs_pin = ADS_CS_PIN,
        .reset_port = ADS_RESET_PORT,
        .reset_pin = ADS_RESET_PIN,
        .start_port = ADS_START_PORT,
        .start_pin = ADS_START_PIN,
        .drdy_port = ADS_DRDY_PORT,
        .drdy_pin = ADS_DRDY_PIN,
    };

    ads1299_t ads;
    ads1299_port_t port = ads1299_wch_make_port(&hw);
    if (ads1299_init(&ads, &port) != ADS1299_OK) {
        fatal("ADS1299_INIT_ERROR\r\n");
    }

    if (ads1299_hardware_reset(&ads) != ADS1299_OK) {
        fatal("ADS1299_RESET_ERROR\r\n");
    }
    Delay_Us(1000u);

    if (ads1299_sdatac(&ads) != ADS1299_OK) {
        fatal("ADS1299_SDATAC_ERROR\r\n");
    }

    uint8_t id = 0u;
    if (ads1299_read_register(&ads, ADS1299_REG_ID, &id) != ADS1299_OK) {
        fatal("ADS1299_ID_READ_ERROR\r\n");
    }
    uart_hex8("ADS1299 ID register: ", id);

    if (ads1299_set_data_rate(&ads, ADS1299_DR_250SPS) != ADS1299_OK) {
        fatal("ADS1299_RATE_ERROR\r\n");
    }

    uint8_t config1 = 0u;
    if (ads1299_read_register(&ads, ADS1299_REG_CONFIG1, &config1) != ADS1299_OK) {
        fatal("ADS1299_CONFIG1_READBACK_ERROR\r\n");
    }
    uart_hex8("CONFIG1 readback: ", config1);

    /* The first boot profile deliberately uses the ADS1299 internal test source.
       This is reproducible and does not require electrodes or a human subject. */
    if (ads1299_configure_internal_test(&ads,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        fatal("ADS1299_TEST_CONFIG_ERROR\r\n");
    }

    if (ads1299_rdatac(&ads) != ADS1299_OK) {
        fatal("ADS1299_RDATAC_ERROR\r\n");
    }
    if (ads1299_start(&ads) != ADS1299_OK) {
        fatal("ADS1299_START_ERROR\r\n");
    }

    uart_text("STREAM_BEGIN\r\n");

    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];

    for (;;) {
        if (g_drdy_pending == 0u) {
            continue;
        }

        __disable_irq();
        const uint32_t pending = g_drdy_pending;
        g_drdy_pending = 0u;
        __enable_irq();

        if (pending > 1u) {
            g_dropped_drdy += pending - 1u;
        }
        g_stream_time_us += pending * ADS_SAMPLE_PERIOD_US;

        if (ads1299_read_frame_continuous(&ads, &frame) != ADS1299_OK) {
            uart_text("ADS1299_FRAME_ERROR\r\n");
            continue;
        }

        const uint8_t flags = g_dropped_drdy ? 0x01u : 0x00u;
        const size_t packet_len = ads1299_packet_encode(packet,
                                                        sizeof(packet),
                                                        flags,
                                                        g_sequence++,
                                                        g_stream_time_us,
                                                        frame.status,
                                                        frame.channel);
        if (packet_len == ADS1299_PACKET_SIZE) {
            raw_uart_write(packet, packet_len);
        }
    }
}

static void board_gpio_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO,
                           ENABLE);

    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Pin = ADS_CS_PIN | ADS_RESET_PIN | ADS_START_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio);

    GPIO_SetBits(ADS_CS_PORT, ADS_CS_PIN);
    GPIO_SetBits(ADS_RESET_PORT, ADS_RESET_PIN);
    GPIO_ResetBits(ADS_START_PORT, ADS_START_PIN);

    gpio.GPIO_Pin = ADS_DRDY_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(ADS_DRDY_PORT, &gpio);
}

static void spi1_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; /* SCK, MOSI */
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_6; /* MISO */
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    SPI_InitTypeDef spi = {0};
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_2Edge; /* ADS1299 SPI mode 1 */
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7u;

    SPI_I2S_DeInit(SPI1);
    SPI_Init(SPI1, &spi);
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
    SPI_Cmd(SPI1, ENABLE);
}

static void uart1_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Pin = GPIO_Pin_9; /* USART1 TX */
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10; /* USART1 RX */
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef uart = {0};
    uart.USART_BaudRate = 921600u;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart);
    USART_Cmd(USART1, ENABLE);
}

static void drdy_exti_init(void)
{
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);

    EXTI_InitTypeDef exti = {0};
    exti.EXTI_Line = EXTI_Line7;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    NVIC_InitTypeDef nvic = {0};
    nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1u;
    nvic.NVIC_IRQChannelSubPriority = 0u;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

static void raw_uart_write(const void *data, size_t len)
{
    const uint8_t *bytes = (const uint8_t *)data;
    if (!bytes || len == 0u) {
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
        }
        USART_SendData(USART1, bytes[i]);
    }
}

static void uart_text(const char *text)
{
    if (text) {
        raw_uart_write(text, strlen(text));
    }
}

static void uart_hex8(const char *prefix, uint8_t value)
{
    static const char hex[] = "0123456789ABCDEF";
    char suffix[6] = {
        '0',
        'x',
        hex[(value >> 4) & 0x0Fu],
        hex[value & 0x0Fu],
        '\r',
        '\n',
    };
    uart_text(prefix);
    raw_uart_write(suffix, sizeof(suffix));
}

static void fatal(const char *reason)
{
    uart_text(reason);
    __disable_irq();
    while (1) {
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
        if (g_drdy_pending != UINT32_MAX) {
            ++g_drdy_pending;
        }
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1) {
    }
}
