#include "ads1299.h"
#include "ads1299_port_pico.h"
#include "ads1299_packet.h"

#include "hardware/spi.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"

#include <stdint.h>
#include <stdio.h>

#define ADS_SPI       spi0
#define PIN_MISO      16u
#define PIN_CS        17u
#define PIN_SCLK      18u
#define PIN_MOSI      19u
#define PIN_DRDY      20u
#define PIN_RESET     21u
#define PIN_START     22u

#define HOST_UART     uart0
#define HOST_UART_TX  0u
#define HOST_UART_RX  1u
#define HOST_BAUD     921600u

static volatile uint32_t g_drdy_pending;
static uint32_t g_sequence;
static uint32_t g_dropped;

static void drdy_irq(uint gpio, uint32_t events) {
    if (gpio == PIN_DRDY && (events & GPIO_IRQ_EDGE_FALL)) {
        if (g_drdy_pending != UINT32_MAX) ++g_drdy_pending;
    }
}

static void init_ads1299_bus(void) {
    spi_init(ADS_SPI, 4u * 1000u * 1000u);
    spi_set_format(ADS_SPI, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    gpio_init(PIN_RESET);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_put(PIN_RESET, 1);

    gpio_init(PIN_START);
    gpio_set_dir(PIN_START, GPIO_OUT);
    gpio_put(PIN_START, 0);

    gpio_init(PIN_DRDY);
    gpio_set_dir(PIN_DRDY, GPIO_IN);
    gpio_pull_up(PIN_DRDY);
    gpio_set_irq_enabled_with_callback(PIN_DRDY, GPIO_IRQ_EDGE_FALL, true, drdy_irq);
}

static void init_host_uart(void) {
    uart_init(HOST_UART, HOST_BAUD);
    uart_set_format(HOST_UART, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(HOST_UART, true);
    gpio_set_function(HOST_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(HOST_UART_RX, GPIO_FUNC_UART);
}

static void host_write(const uint8_t *data, size_t len) {
    if (!data || !len) return;
    uart_write_blocking(HOST_UART, data, len);
}

int main(void) {
    stdio_init_all();
    sleep_ms(250);
    init_host_uart();
    init_ads1299_bus();

    printf("ADS1299-Core RP2040 Pico reference project\n");
    printf("SPI0 Mode 1 @ 4 MHz; UART0 binary stream @ %u baud\n", HOST_BAUD);

    ads1299_pico_ctx_t hw = {
        .spi = ADS_SPI,
        .cs_pin = PIN_CS,
        .reset_pin = (int)PIN_RESET,
        .start_pin = (int)PIN_START,
        .drdy_pin = (int)PIN_DRDY,
    };

    ads1299_t ads;
    ads1299_port_t port = ads1299_pico_make_port(&hw);
    if (ads1299_init(&ads, &port) != ADS1299_OK) {
        printf("ERROR: ads1299_init\n");
        return 1;
    }
    if (ads1299_hardware_reset(&ads) != ADS1299_OK) {
        printf("ERROR: hardware reset\n");
        return 1;
    }
    sleep_ms(2);
    if (ads1299_sdatac(&ads) != ADS1299_OK) {
        printf("ERROR: SDATAC\n");
        return 1;
    }

    uint8_t id = 0;
    if (ads1299_read_register(&ads, ADS1299_REG_ID, &id) != ADS1299_OK) {
        printf("ERROR: read ID\n");
        return 1;
    }
    printf("ADS1299 ID register: 0x%02X\n", id);

    if (ads1299_set_data_rate(&ads, ADS1299_DR_250SPS) != ADS1299_OK) return 1;
    if (ads1299_configure_internal_test(&ads, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        printf("ERROR: internal test configuration\n");
        return 1;
    }
    if (ads1299_rdatac(&ads) != ADS1299_OK) return 1;
    if (ads1299_start(&ads) != ADS1299_OK) return 1;

    printf("STREAM_BEGIN: binary packets are on UART0 TX GP0\n");

    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];
    for (;;) {
        if (!g_drdy_pending) {
            tight_loop_contents();
            continue;
        }

        uint32_t irq_state = save_and_disable_interrupts();
        uint32_t pending = g_drdy_pending;
        g_drdy_pending = 0;
        restore_interrupts(irq_state);
        if (pending > 1u) g_dropped += pending - 1u;

        if (ads1299_read_frame_continuous(&ads, &frame) != ADS1299_OK) {
            printf("WARN: frame read failed\n");
            continue;
        }

        uint8_t flags = g_dropped ? 0x01u : 0x00u;
        size_t n = ads1299_packet_encode(packet, sizeof(packet), flags,
                                         g_sequence++, time_us_32(),
                                         frame.status, frame.channel);
        if (n == ADS1299_PACKET_SIZE) host_write(packet, n);
    }
}
