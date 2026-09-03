#include "ads1299.h"
#include "ads1299_port_esp_idf.h"
#include "ads1299_packet.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define ADS_SPI_HOST   SPI2_HOST
#define PIN_SCLK       GPIO_NUM_12
#define PIN_MOSI       GPIO_NUM_11
#define PIN_MISO       GPIO_NUM_13
#define PIN_CS         GPIO_NUM_10
#define PIN_DRDY       GPIO_NUM_9
#define PIN_RESET      GPIO_NUM_8
#define PIN_START      GPIO_NUM_7

static volatile uint32_t s_drdy_pending;
static uint32_t s_sequence;
static uint32_t s_dropped;

static void IRAM_ATTR drdy_isr(void *arg) {
    (void)arg;
    if (s_drdy_pending != UINT32_MAX) ++s_drdy_pending;
}

static void raw_uart_write(const void *data, size_t len) {
    if (!data || !len) return;
    (void)uart_write_bytes(UART_NUM_0, data, len);
}

static void configure_uart(void) {
    uart_config_t cfg = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 4096, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &cfg));
}

static spi_device_handle_t configure_spi(void) {
    spi_bus_config_t bus = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(ADS_SPI_HOST, &bus, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4 * 1000 * 1000,
        .mode = 1,
        .spics_io_num = -1, /* common driver owns CS */
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX ? 0 : 0,
    };
    spi_device_handle_t dev = NULL;
    ESP_ERROR_CHECK(spi_bus_add_device(ADS_SPI_HOST, &devcfg, &dev));
    return dev;
}

static void configure_gpio(void) {
    gpio_config_t out = {
        .pin_bit_mask = (1ULL << PIN_CS) | (1ULL << PIN_RESET) | (1ULL << PIN_START),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&out));
    gpio_set_level(PIN_CS, 1);
    gpio_set_level(PIN_RESET, 1);
    gpio_set_level(PIN_START, 0);

    gpio_config_t in = {
        .pin_bit_mask = 1ULL << PIN_DRDY,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&in));
    esp_err_t e = gpio_install_isr_service(0);
    if (e != ESP_OK && e != ESP_ERR_INVALID_STATE) ESP_ERROR_CHECK(e);
    ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_DRDY, drdy_isr, NULL));
}

void app_main(void) {
    configure_uart();
    configure_gpio();
    spi_device_handle_t spi = configure_spi();

    const char *banner = "\r\nADS1299-Core ESP32-S3 reference project\r\n";
    raw_uart_write(banner, strlen(banner));

    ads1299_esp_idf_ctx_t hw = {
        .spi = spi,
        .cs_pin = PIN_CS,
        .reset_pin = PIN_RESET,
        .start_pin = PIN_START,
        .drdy_pin = PIN_DRDY,
    };
    ads1299_port_t port = ads1299_esp_idf_make_port(&hw);
    ads1299_t ads;
    if (ads1299_init(&ads, &port) != ADS1299_OK) abort();
    if (ads1299_hardware_reset(&ads) != ADS1299_OK) abort();
    vTaskDelay(pdMS_TO_TICKS(2));
    if (ads1299_sdatac(&ads) != ADS1299_OK) abort();

    uint8_t id = 0;
    if (ads1299_read_register(&ads, ADS1299_REG_ID, &id) != ADS1299_OK) abort();
    char line[48];
    int nline = snprintf(line, sizeof(line), "ADS1299 ID register: 0x%02X\r\n", id);
    if (nline > 0) raw_uart_write(line, (size_t)nline);

    if (ads1299_set_data_rate(&ads, ADS1299_DR_250SPS) != ADS1299_OK) abort();
    if (ads1299_configure_internal_test(&ads, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) abort();
    if (ads1299_rdatac(&ads) != ADS1299_OK) abort();
    if (ads1299_start(&ads) != ADS1299_OK) abort();

    const char *start = "STREAM_BEGIN\r\n";
    raw_uart_write(start, strlen(start));

    ads1299_frame_t frame;
    uint8_t packet[ADS1299_PACKET_SIZE];
    for (;;) {
        if (!s_drdy_pending) {
            taskYIELD();
            continue;
        }

        portENTER_CRITICAL(NULL);
        uint32_t pending = s_drdy_pending;
        s_drdy_pending = 0;
        portEXIT_CRITICAL(NULL);
        if (pending > 1u) s_dropped += pending - 1u;

        if (ads1299_read_frame_continuous(&ads, &frame) != ADS1299_OK) continue;
        uint8_t flags = s_dropped ? 0x01u : 0x00u;
        size_t n = ads1299_packet_encode(packet, sizeof(packet), flags,
                                         s_sequence++, (uint32_t)esp_timer_get_time(),
                                         frame.status, frame.channel);
        if (n == ADS1299_PACKET_SIZE) raw_uart_write(packet, n);
    }
}
