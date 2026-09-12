#include <inttypes.h>

#include "ads1299.h"
#include "ads1299_espidf_hal.h"
#include "esp32c2_frame_queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ads1299-c2";

static ads1299_espidf_hal_ctx_t g_idf_hal;
static ads1299_platform_hal_t g_hal;
static ads1299_mcu_port_t g_mcu;
static ads1299_port_t g_port;
static ads1299_t g_device;
static esp32c2_ads1299_frame_queue_t g_queue;
static portMUX_TYPE g_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t g_sequence;

static void acquisition_task(void *arg)
{
    (void)arg;
    ads1299_frame_t frame;

    for (;;) {
        if (g_port.drdy_read(g_port.user) == 0) {
            if (ads1299_read_frame_continuous(&g_device, &frame) != ADS1299_OK) {
                ESP_LOGE(TAG, "frame read failed; acquisition task stopping");
                break;
            }

            const uint32_t timestamp_us = (uint32_t)esp_timer_get_time();
            const uint32_t sequence = g_sequence++;
            portENTER_CRITICAL(&g_queue_lock);
            (void)esp32c2_ads1299_frame_queue_push(
                &g_queue, &frame, timestamp_us, sequence);
            portEXIT_CRITICAL(&g_queue_lock);
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    (void)ads1299_stop(&g_device);
    (void)ads1299_sdatac(&g_device);
    vTaskDelete(NULL);
}

static void transport_task(void *arg)
{
    (void)arg;
    esp32c2_ads1299_frame_record_t record;
    uint32_t last_reported_dropped = 0u;

    for (;;) {
        bool have_record;
        uint32_t dropped;
        size_t high_watermark;

        portENTER_CRITICAL(&g_queue_lock);
        have_record = esp32c2_ads1299_frame_queue_pop(&g_queue, &record);
        dropped = esp32c2_ads1299_frame_queue_dropped(&g_queue);
        high_watermark = esp32c2_ads1299_frame_queue_high_watermark(&g_queue);
        portEXIT_CRITICAL(&g_queue_lock);

        if (dropped != last_reported_dropped) {
            ESP_LOGW(TAG,
                     "acquisition queue overflow: dropped=%" PRIu32 " high_watermark=%u/%u",
                     dropped,
                     (unsigned)high_watermark,
                     (unsigned)ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);
            last_reported_dropped = dropped;
        }

        if (!have_record) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        /*
         * This is the deliberately lower-priority transport side. Replace the
         * ESP_LOGI call with UART/Wi-Fi/BLE/canonical-packet transmission as
         * needed; the DRDY/SPI acquisition task remains free of that latency.
         */
        if (record.sequence < 8u || (record.sequence % 250u) == 0u) {
            ESP_LOGI(TAG,
                     "frame seq=%" PRIu32 " t=%" PRIu32
                     "us status=%02x%02x%02x ch1=%" PRId32,
                     record.sequence,
                     record.timestamp_us,
                     record.frame.status[0],
                     record.frame.status[1],
                     record.frame.status[2],
                     record.frame.channel[0]);
        }
    }
}

void app_main(void)
{
    ads1299_device_id_t identity;

    int rc = ads1299_espidf_hal_init(&g_idf_hal, &g_hal);
    if (rc != 0) {
        ESP_LOGE(TAG, "ESP-IDF HAL init failed: %d", rc);
        return;
    }
    if (ads1299_mcu_port_init(&g_mcu, &g_hal) != 0) {
        ESP_LOGE(TAG, "ADS1299 portable port init failed");
        return;
    }

    g_port = ads1299_mcu_make_port(&g_mcu);
    if (ads1299_init(&g_device, &g_port) != ADS1299_OK) {
        ESP_LOGE(TAG, "core init failed");
        return;
    }
    if (ads1299_hardware_reset(&g_device) != ADS1299_OK ||
        ads1299_sdatac(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "reset/SDATAC failed");
        return;
    }

    if (ads1299_read_device_id(&g_device, &identity) != ADS1299_OK) {
        ESP_LOGE(TAG, "ID read failed: check power, wiring and SPI mode 1");
        return;
    }
    if (!identity.is_ads1299_family) {
        ESP_LOGE(TAG, "unexpected ID 0x%02x: ADS1299 family not detected", identity.raw);
        return;
    }
    ESP_LOGI(TAG, "probe OK: ADS1299-family ID=0x%02x", identity.raw);

    if (ads1299_configure_internal_test(&g_device,
                                        ADS1299_GAIN_24,
                                        0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) {
        ESP_LOGE(TAG, "internal-test configuration failed");
        return;
    }
    if (ads1299_rdatac(&g_device) != ADS1299_OK ||
        ads1299_start(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "continuous start failed");
        return;
    }

    esp32c2_ads1299_frame_queue_init(&g_queue);
    g_sequence = 0u;

    ESP_LOGI(TAG,
             "internal-test streaming started: bounded queue=%u frames",
             (unsigned)ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);

    if (xTaskCreate(transport_task, "ads1299_tx", 3072, NULL, 3, NULL) != pdPASS) {
        ESP_LOGE(TAG, "transport task creation failed");
        (void)ads1299_stop(&g_device);
        (void)ads1299_sdatac(&g_device);
        return;
    }
    if (xTaskCreate(acquisition_task, "ads1299_acq", 3072, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "acquisition task creation failed");
        (void)ads1299_stop(&g_device);
        (void)ads1299_sdatac(&g_device);
        return;
    }

    ESP_LOGI(TAG,
             "acquisition runs above transport priority; slow UART/Wi-Fi/BLE work must stay in transport task");
}
