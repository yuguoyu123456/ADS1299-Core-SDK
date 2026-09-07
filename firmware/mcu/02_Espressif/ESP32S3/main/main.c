#include "ads1299.h"
#include "ads1299_drdy.h"
#include "ads1299_esp_idf_hal.h"
#include "ads1299_spi.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADS_SPI_HOST SPI2_HOST
#define PIN_SCLK     GPIO_NUM_12
#define PIN_MOSI     GPIO_NUM_11
#define PIN_MISO     GPIO_NUM_13
#define PIN_CS       GPIO_NUM_10
#define PIN_DRDY     GPIO_NUM_9
#define PIN_RESET    GPIO_NUM_8
#define PIN_START    GPIO_NUM_7
#define PIN_PWDN     GPIO_NUM_6

static spi_device_handle_t configure_spi(void) {
    const spi_bus_config_t bus = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    const spi_device_interface_config_t device_config = {
        .clock_speed_hz = 4 * 1000 * 1000,
        .mode = 1,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    spi_device_handle_t device = NULL;

    ESP_ERROR_CHECK(spi_bus_initialize(ADS_SPI_HOST, &bus, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(ADS_SPI_HOST, &device_config, &device));
    return device;
}

static void configure_gpio(void) {
    const gpio_config_t outputs = {
        .pin_bit_mask = (1ULL << PIN_CS) | (1ULL << PIN_RESET) |
                        (1ULL << PIN_PWDN) | (1ULL << PIN_START),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    const gpio_config_t input = {
        .pin_bit_mask = 1ULL << PIN_DRDY,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&outputs));
    ESP_ERROR_CHECK(gpio_config(&input));
    ESP_ERROR_CHECK(gpio_set_level(PIN_CS, 1));
    ESP_ERROR_CHECK(gpio_set_level(PIN_RESET, 1));
    ESP_ERROR_CHECK(gpio_set_level(PIN_PWDN, 1));
    ESP_ERROR_CHECK(gpio_set_level(PIN_START, 0));
}

void app_main(void) {
    ads1299_esp_idf_ctx_t hardware;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu_port;
    ads1299_t device;
    ads1299_device_id_t identity;
    ads1299_frame_t frame;

    configure_gpio();
    hardware = (ads1299_esp_idf_ctx_t){
        .spi = configure_spi(),
        .cs_pin = PIN_CS,
        .reset_pin = PIN_RESET,
        .pwdn_pin = PIN_PWDN,
        .start_pin = PIN_START,
        .drdy_pin = PIN_DRDY,
    };

    ESP_ERROR_CHECK(ads1299_esp_idf_make_hal(&hal, &hardware) == 0
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_mcu_port_init(&mcu_port, &hal) == 0
                        ? ESP_OK : ESP_FAIL);
    ads1299_port_t core_port = ads1299_mcu_make_port(&mcu_port);

    ESP_ERROR_CHECK(ads1299_init(&device, &core_port) == ADS1299_OK
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_hardware_reset(&device) == ADS1299_OK
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_sdatac(&device) == ADS1299_OK
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_read_device_id(&device, &identity) == ADS1299_OK &&
                        identity.is_ads1299_family ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_set_data_rate(&device, ADS1299_DR_250SPS) == ADS1299_OK
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_configure_internal_test(
                        &device, ADS1299_GAIN_24, 0,
                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) == ADS1299_OK
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_rdatac(&device) == ADS1299_OK ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_start(&device) == ADS1299_OK ? ESP_OK : ESP_FAIL);

    for (;;) {
        if (ads1299_mcu_data_ready(&mcu_port)) {
            ESP_ERROR_CHECK(ads1299_read_frame_continuous(&device, &frame) ==
                                ADS1299_OK ? ESP_OK : ESP_FAIL);
            /* Pass frame to a firmware/transport backend outside this example. */
        } else {
            taskYIELD();
        }
    }
}
