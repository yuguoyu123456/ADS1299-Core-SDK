#include "board_sdk.h"

#include "gd32f4xx.h"

#define ADS_SCK_PORT GPIOA
#define ADS_SCK_PIN GPIO_PIN_5
#define ADS_MISO_PORT GPIOA
#define ADS_MISO_PIN GPIO_PIN_6
#define ADS_MOSI_PORT GPIOA
#define ADS_MOSI_PIN GPIO_PIN_7
#define ADS_CS_PORT GPIOB
#define ADS_CS_PIN GPIO_PIN_0
#define ADS_DRDY_PORT GPIOB
#define ADS_DRDY_PIN GPIO_PIN_1
#define ADS_RESET_PORT GPIOB
#define ADS_RESET_PIN GPIO_PIN_2
#define ADS_PWDN_PORT GPIOB
#define ADS_PWDN_PIN GPIO_PIN_10
#define ADS_START_PORT GPIOB
#define ADS_START_PIN GPIO_PIN_11
#define STREAM_TX_PORT GPIOA
#define STREAM_TX_PIN GPIO_PIN_9

static void pin_location(board_sdk_pin_t pin, uint32_t *port, uint32_t *mask)
{
    switch (pin) {
    case BOARD_PIN_SCK:   *port = ADS_SCK_PORT;   *mask = ADS_SCK_PIN; break;
    case BOARD_PIN_MISO:  *port = ADS_MISO_PORT;  *mask = ADS_MISO_PIN; break;
    case BOARD_PIN_MOSI:  *port = ADS_MOSI_PORT;  *mask = ADS_MOSI_PIN; break;
    case BOARD_PIN_CS:    *port = ADS_CS_PORT;    *mask = ADS_CS_PIN; break;
    case BOARD_PIN_DRDY:  *port = ADS_DRDY_PORT;  *mask = ADS_DRDY_PIN; break;
    case BOARD_PIN_RESET: *port = ADS_RESET_PORT; *mask = ADS_RESET_PIN; break;
    case BOARD_PIN_PWDN:  *port = ADS_PWDN_PORT;  *mask = ADS_PWDN_PIN; break;
    case BOARD_PIN_START: *port = ADS_START_PORT; *mask = ADS_START_PIN; break;
    case BOARD_PIN_TX:    *port = STREAM_TX_PORT; *mask = STREAM_TX_PIN; break;
    default:               *port = ADS_SCK_PORT;   *mask = ADS_SCK_PIN; break;
    }
}

static void configure_output(uint32_t port, uint32_t pin)
{
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
}

static void configure_input(uint32_t port, uint32_t pin)
{
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin);
}

int board_sdk_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    configure_output(ADS_SCK_PORT, ADS_SCK_PIN);
    configure_input(ADS_MISO_PORT, ADS_MISO_PIN);
    configure_output(ADS_MOSI_PORT, ADS_MOSI_PIN);
    configure_output(ADS_CS_PORT, ADS_CS_PIN);
    configure_input(ADS_DRDY_PORT, ADS_DRDY_PIN);
    configure_output(ADS_RESET_PORT, ADS_RESET_PIN);
    configure_output(ADS_PWDN_PORT, ADS_PWDN_PIN);
    configure_output(ADS_START_PORT, ADS_START_PIN);
    configure_output(STREAM_TX_PORT, STREAM_TX_PIN);

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0u;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    return 0;
}

void board_sdk_write_pin(board_sdk_pin_t pin, int level)
{
    uint32_t port;
    uint32_t mask;
    pin_location(pin, &port, &mask);
    gpio_bit_write(port, mask, level ? SET : RESET);
}

int board_sdk_read_pin(board_sdk_pin_t pin)
{
    uint32_t port;
    uint32_t mask;
    pin_location(pin, &port, &mask);
    return gpio_input_bit_get(port, mask) == SET ? 1 : 0;
}

void board_sdk_delay_us(uint32_t us)
{
    const uint32_t cycles_per_us = SystemCoreClock / 1000000u;
    const uint32_t start = DWT->CYCCNT;
    const uint32_t wait_cycles = us * cycles_per_us;
    while ((uint32_t)(DWT->CYCCNT - start) < wait_cycles) {
    }
}

void board_sdk_fatal(const char *reason)
{
    (void)reason;
    for (;;) {
        gpio_bit_write(STREAM_TX_PORT, STREAM_TX_PIN, RESET);
    }
}
