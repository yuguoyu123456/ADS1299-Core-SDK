#include "ads1299.h"
#include "ads1299_drdy.h"
#include "ads1299_esp_idf_hal.h"
#include "ads1299_spi.h"
#include "ads1299_stream_queue.h"
#include "esp32s3_devkitc1_ads1299.h"

#include <inttypes.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ads1299_s3";

static ads1299_esp_idf_ctx_t s_hardware;
static ads1299_platform_hal_t s_hal;
static ads1299_mcu_port_t s_mcu_port;
static ads1299_t s_device;
static esp32s3_ads1299_stream_queue_t s_stream_queue;
static portMUX_TYPE s_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static TaskHandle_t s_acquisition_task;
static uint32_t s_sequence;

static spi_device_handle_t configure_spi(void) {
    const spi_bus_config_t bus = {
        .mosi_io_num = ADS1299_BOARD_PIN_MOSI,
        .miso_io_num = ADS1299_BOARD_PIN_MISO,
        .sclk_io_num = ADS1299_BOARD_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    const spi_device_interface_config_t device_config = {
        .clock_speed_hz = ADS1299_BOARD_SPI_HZ,
        .mode = 1,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    spi_device_handle_t device = NULL;

    ESP_ERROR_CHECK(spi_bus_initialize(ADS1299_BOARD_SPI_HOST, &bus, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(ADS1299_BOARD_SPI_HOST, &device_config, &device));
    return device;
}

static void configure_gpio(void) {
    const gpio_config_t outputs = {
        .pin_bit_mask = (1ULL << ADS1299_BOARD_PIN_CS) |
                        (1ULL << ADS1299_BOARD_PIN_RESET) |
                        (1ULL << ADS1299_BOARD_PIN_PWDN) |
                        (1ULL << ADS1299_BOARD_PIN_START),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    const gpio_config_t input = {
        .pin_bit_mask = 1ULL << ADS1299_BOARD_PIN_DRDY,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&outputs));
    ESP_ERROR_CHECK(gpio_config(&input));
    ESP_ERROR_CHECK(gpio_set_level(ADS1299_BOARD_PIN_CS, 1));
    ESP_ERROR_CHECK(gpio_set_level(ADS1299_BOARD_PIN_RESET, 1));
    ESP_ERROR_CHECK(gpio_set_level(ADS1299_BOARD_PIN_PWDN, 1));
    ESP_ERROR_CHECK(gpio_set_level(ADS1299_BOARD_PIN_START, 0));
}

static esp_err_t ads_status(ads1299_status_t status) {
    return status == ADS1299_OK ? ESP_OK : ESP_FAIL;
}

static esp_err_t capture_frames(const char *label, size_t frame_count) {
    for (size_t i = 0; i < frame_count; ++i) {
        const int64_t deadline = esp_timer_get_time() +
            ((int64_t)ADS1299_BOARD_DRDY_TIMEOUT_MS * 1000LL);
        while (!ads1299_mcu_data_ready(&s_mcu_port)) {
            if (esp_timer_get_time() >= deadline) {
                ESP_LOGE(TAG, "%s: DRDY timeout at frame %u", label, (unsigned)i);
                return ESP_ERR_TIMEOUT;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        ads1299_frame_t frame;
        if (ads1299_read_frame_continuous(&s_device, &frame) != ADS1299_OK) {
            ESP_LOGE(TAG, "%s: frame-read failure at frame %u", label, (unsigned)i);
            return ESP_FAIL;
        }
    }
    ESP_LOGI(TAG, "%s: %u frames captured", label, (unsigned)frame_count);
    return ESP_OK;
}

static esp_err_t run_finite_profile(const char *label, size_t frame_count) {
    if (ads1299_rdatac(&s_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "%s: RDATAC failed", label);
        return ESP_FAIL;
    }
    if (ads1299_start(&s_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "%s: START failed", label);
        return ESP_FAIL;
    }

    const esp_err_t result = capture_frames(label, frame_count);
    const ads1299_status_t stop_status = ads1299_stop(&s_device);
    const ads1299_status_t sdatac_status = ads1299_sdatac(&s_device);
    if (result != ESP_OK) {
        return result;
    }
    if (stop_status != ADS1299_OK || sdatac_status != ADS1299_OK) {
        ESP_LOGE(TAG, "%s: STOP/SDATAC failed", label);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static int queue_push(const esp32s3_ads1299_stream_item_t *item) {
    int result;
    portENTER_CRITICAL(&s_queue_lock);
    result = esp32s3_ads1299_stream_queue_push(&s_stream_queue, item);
    portEXIT_CRITICAL(&s_queue_lock);
    return result;
}

static int queue_pop(esp32s3_ads1299_stream_item_t *item) {
    int result;
    portENTER_CRITICAL(&s_queue_lock);
    result = esp32s3_ads1299_stream_queue_pop(&s_stream_queue, item);
    portEXIT_CRITICAL(&s_queue_lock);
    return result;
}

static void IRAM_ATTR drdy_isr(void *arg) {
    (void)arg;
    BaseType_t higher_priority_task_woken = pdFALSE;
    if (s_acquisition_task != NULL) {
        vTaskNotifyGiveFromISR(s_acquisition_task, &higher_priority_task_woken);
    }
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void acquisition_task(void *arg) {
    (void)arg;
    for (;;) {
        const uint32_t notifications = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(
            ADS1299_BOARD_DRDY_TIMEOUT_MS));
        if (notifications == 0u) {
            ESP_LOGW(TAG, "stream: DRDY timeout");
            continue;
        }

        esp32s3_ads1299_stream_item_t item = {
            .timestamp_us = (uint64_t)esp_timer_get_time(),
            .sequence = s_sequence++,
        };
        if (ads1299_read_frame_continuous(&s_device, &item.frame) != ADS1299_OK) {
            ESP_LOGE(TAG, "stream: frame-read failure");
            continue;
        }

        const int push_result = queue_push(&item);
        if (push_result == 0) {
            uint32_t dropped;
            size_t high_watermark;
            portENTER_CRITICAL(&s_queue_lock);
            dropped = esp32s3_ads1299_stream_queue_dropped(&s_stream_queue);
            high_watermark = esp32s3_ads1299_stream_queue_high_watermark(&s_stream_queue);
            portEXIT_CRITICAL(&s_queue_lock);
            ESP_LOGW(TAG, "stream: queue overflow dropped=%" PRIu32 " high=%u/%u",
                     dropped, (unsigned)high_watermark,
                     (unsigned)ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY);
        }
    }
}

static void transport_task(void *arg) {
    (void)arg;
    uint32_t last_reported_sequence = 0u;
    for (;;) {
        esp32s3_ads1299_stream_item_t item;
        if (queue_pop(&item) == 1) {
            /* Replace this sparse log with UART/Wi-Fi/BLE transport. Keep that
             * work here, never in the DRDY/acquisition path. */
            if ((item.sequence - last_reported_sequence) >= 250u) {
                size_t depth;
                size_t high_watermark;
                uint32_t dropped;
                portENTER_CRITICAL(&s_queue_lock);
                depth = esp32s3_ads1299_stream_queue_count(&s_stream_queue);
                high_watermark = esp32s3_ads1299_stream_queue_high_watermark(&s_stream_queue);
                dropped = esp32s3_ads1299_stream_queue_dropped(&s_stream_queue);
                portEXIT_CRITICAL(&s_queue_lock);
                ESP_LOGI(TAG,
                         "EEG250 seq=%" PRIu32 " ts=%" PRIu64
                         " queue=%u high=%u dropped=%" PRIu32,
                         item.sequence, item.timestamp_us, (unsigned)depth,
                         (unsigned)high_watermark, dropped);
                last_reported_sequence = item.sequence;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

static void configure_eeg_250(void) {
    ESP_ERROR_CHECK(ads_status(ads1299_set_data_rate(&s_device, ADS1299_DR_250SPS)));
    ESP_ERROR_CHECK(ads_status(ads1299_set_srb1(&s_device, 0)));

    const uint8_t channel_count = s_device.channel_count == 0u ? 8u : s_device.channel_count;
    for (uint8_t channel = 1u; channel <= channel_count; ++channel) {
        ESP_ERROR_CHECK(ads_status(ads1299_set_channel(
            &s_device, channel, ADS1299_GAIN_24, ADS1299_MUX_NORMAL, 0, 0)));
    }
    ESP_LOGI(TAG, "EEG profile: 250 SPS, gain 24, normal input, SRB1/SRB2 off");
}

void app_main(void) {
    ads1299_device_id_t identity;

    configure_gpio();
    s_hardware = (ads1299_esp_idf_ctx_t){
        .spi = configure_spi(),
        .cs_pin = ADS1299_BOARD_PIN_CS,
        .reset_pin = ADS1299_BOARD_PIN_RESET,
        .pwdn_pin = ADS1299_BOARD_PIN_PWDN,
        .start_pin = ADS1299_BOARD_PIN_START,
        .drdy_pin = ADS1299_BOARD_PIN_DRDY,
    };

    ESP_ERROR_CHECK(ads1299_esp_idf_make_hal(&s_hal, &s_hardware) == 0
                        ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(ads1299_mcu_port_init(&s_mcu_port, &s_hal) == 0
                        ? ESP_OK : ESP_FAIL);
    ads1299_port_t core_port = ads1299_mcu_make_port(&s_mcu_port);

    ESP_ERROR_CHECK(ads_status(ads1299_init(&s_device, &core_port)));
    ESP_ERROR_CHECK(ads_status(ads1299_hardware_reset(&s_device)));
    ESP_ERROR_CHECK(ads_status(ads1299_sdatac(&s_device)));
    ESP_ERROR_CHECK(ads1299_read_device_id(&s_device, &identity) == ADS1299_OK &&
                        identity.is_ads1299_family ? ESP_OK : ESP_FAIL);
    ESP_LOGI(TAG, "probe: ADS1299 family detected, channels=%u",
             (unsigned)s_device.channel_count);

    ESP_ERROR_CHECK(ads_status(ads1299_set_data_rate(&s_device, ADS1299_DR_250SPS)));
    ESP_ERROR_CHECK(ads_status(ads1299_configure_internal_test(
        &s_device, ADS1299_GAIN_24, 0, ADS1299_TEST_FREQ_FCLK_DIV_2_21)));
    ESP_ERROR_CHECK(run_finite_profile("internal-test", 8u));

    ESP_ERROR_CHECK(ads_status(ads1299_configure_input_short_test(
        &s_device, ADS1299_GAIN_24)));
    ESP_ERROR_CHECK(run_finite_profile("input-short", 8u));

    configure_eeg_250();
    esp32s3_ads1299_stream_queue_init(&s_stream_queue);

    BaseType_t task_ok = xTaskCreate(acquisition_task, "ads1299_acq", 4096, NULL,
                                     configMAX_PRIORITIES - 2, &s_acquisition_task);
    ESP_ERROR_CHECK(task_ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);
    task_ok = xTaskCreate(transport_task, "ads1299_transport", 4096, NULL,
                          tskIDLE_PRIORITY + 2, NULL);
    ESP_ERROR_CHECK(task_ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(ADS1299_BOARD_PIN_DRDY, drdy_isr, NULL));

    ESP_ERROR_CHECK(ads_status(ads1299_rdatac(&s_device)));
    ESP_ERROR_CHECK(ads_status(ads1299_start(&s_device)));
    ESP_LOGI(TAG, "stream: event-driven bounded EEG250 acquisition started");
}
