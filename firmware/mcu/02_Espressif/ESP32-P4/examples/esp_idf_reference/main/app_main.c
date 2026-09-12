#include <inttypes.h>
#include <stdbool.h>

#include "ads1299.h"
#include "ads1299_espidf_hal.h"
#include "esp32p4_frame_queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ads1299-p4";
static ads1299_espidf_hal_ctx_t g_idf_hal;
static ads1299_platform_hal_t g_hal;
static ads1299_mcu_port_t g_mcu;
static ads1299_port_t g_port;
static ads1299_t g_device;
static esp32p4_ads1299_frame_queue_t g_queue;
static portMUX_TYPE g_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t g_sequence;

#define DIAG_FRAMES 8u
#define DRDY_TIMEOUT_US 500000u
#define DRDY_POLL_US 100u

static bool start_continuous(const char *phase)
{
    ads1299_status_t a = ads1299_rdatac(&g_device);
    ads1299_status_t b = ads1299_start(&g_device);
    if (a != ADS1299_OK || b != ADS1299_OK) {
        ESP_LOGE(TAG, "%s start failed: rdatac=%d start=%d", phase, (int)a, (int)b);
        return false;
    }
    return true;
}

static bool stop_continuous(const char *phase)
{
    ads1299_status_t a = ads1299_stop(&g_device);
    ads1299_status_t b = ads1299_sdatac(&g_device);
    if (a != ADS1299_OK || b != ADS1299_OK) {
        ESP_LOGE(TAG, "%s stop failed: stop=%d sdatac=%d", phase, (int)a, (int)b);
        return false;
    }
    return true;
}

static bool capture_frames(const char *phase)
{
    ads1299_frame_t frame;
    for (uint32_t i = 0; i < DIAG_FRAMES; ++i) {
        ads1299_status_t rc = ads1299_wait_drdy(&g_device, DRDY_TIMEOUT_US, DRDY_POLL_US);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s DRDY timeout at frame %" PRIu32, phase, i);
            return false;
        }
        rc = ads1299_read_frame_continuous(&g_device, &frame);
        if (rc != ADS1299_OK) {
            ESP_LOGE(TAG, "%s frame read failed at frame %" PRIu32, phase, i);
            return false;
        }
        ESP_LOGI(TAG, "%s frame=%" PRIu32 " status=%02x%02x%02x ch1=%" PRId32,
                 phase, i, frame.status[0], frame.status[1], frame.status[2], frame.channel[0]);
    }
    return true;
}

static bool run_internal_test(void)
{
    if (ads1299_configure_internal_test(&g_device, ADS1299_GAIN_24, 0,
                                        ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return false;
    if (!start_continuous("internal-test")) return false;
    bool ok = capture_frames("internal-test");
    return stop_continuous("internal-test") && ok;
}

static bool run_input_short(void)
{
    if (ads1299_configure_input_short_test(&g_device, ADS1299_GAIN_24) != ADS1299_OK) return false;
    if (!start_continuous("input-short")) return false;
    bool ok = capture_frames("input-short");
    return stop_continuous("input-short") && ok;
}

static bool configure_eeg250(uint8_t channels)
{
    if (ads1299_set_data_rate(&g_device, ADS1299_DR_250SPS) != ADS1299_OK) return false;
    if (ads1299_set_srb1(&g_device, 0) != ADS1299_OK) return false;
    for (uint8_t ch = 1u; ch <= channels; ++ch) {
        if (ads1299_set_channel(&g_device, ch, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) return false;
    }
    return true;
}

static void acquisition_task(void *arg)
{
    (void)arg;
    ads1299_frame_t frame;
    for (;;) {
        if (g_port.drdy_read(g_port.user) == 0) {
            if (ads1299_read_frame_continuous(&g_device, &frame) != ADS1299_OK) {
                ESP_LOGE(TAG, "EEG250 frame read failed; stopping acquisition");
                break;
            }
            uint32_t ts = (uint32_t)esp_timer_get_time();
            uint32_t seq = g_sequence++;
            portENTER_CRITICAL(&g_queue_lock);
            (void)esp32p4_ads1299_frame_queue_push(&g_queue, &frame, ts, seq);
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
    esp32p4_ads1299_frame_record_t rec;
    uint32_t last_dropped = 0u;
    for (;;) {
        bool have;
        uint32_t dropped;
        size_t high;
        size_t depth;
        portENTER_CRITICAL(&g_queue_lock);
        have = esp32p4_ads1299_frame_queue_pop(&g_queue, &rec);
        dropped = esp32p4_ads1299_frame_queue_dropped(&g_queue);
        high = esp32p4_ads1299_frame_queue_high_watermark(&g_queue);
        depth = esp32p4_ads1299_frame_queue_count(&g_queue);
        portEXIT_CRITICAL(&g_queue_lock);

        if (dropped != last_dropped) {
            ESP_LOGW(TAG, "queue overflow dropped=%" PRIu32 " depth=%u high=%u/%u",
                     dropped, (unsigned)depth, (unsigned)high,
                     (unsigned)ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY);
            last_dropped = dropped;
        }
        if (!have) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }
        if (rec.sequence < 8u || (rec.sequence % 250u) == 0u) {
            ESP_LOGI(TAG, "EEG250 seq=%" PRIu32 " t=%" PRIu32 " status=%02x%02x%02x ch1=%" PRId32,
                     rec.sequence, rec.timestamp_us,
                     rec.frame.status[0], rec.frame.status[1], rec.frame.status[2], rec.frame.channel[0]);
        }
    }
}

void app_main(void)
{
    ads1299_device_id_t id;
    int rc = ads1299_espidf_hal_init(&g_idf_hal, &g_hal);
    if (rc != 0) { ESP_LOGE(TAG, "HAL init failed: %d", rc); return; }
    if (ads1299_mcu_port_init(&g_mcu, &g_hal) != 0) { ESP_LOGE(TAG, "port init failed"); return; }
    g_port = ads1299_mcu_make_port(&g_mcu);
    if (ads1299_init(&g_device, &g_port) != ADS1299_OK) { ESP_LOGE(TAG, "core init failed"); return; }
    if (ads1299_hardware_reset(&g_device) != ADS1299_OK || ads1299_sdatac(&g_device) != ADS1299_OK) {
        ESP_LOGE(TAG, "reset/SDATAC failed"); return;
    }
    if (ads1299_read_device_id(&g_device, &id) != ADS1299_OK || !id.is_ads1299_family) {
        ESP_LOGE(TAG, "probe failed: check power/wiring/SPI mode 1"); return;
    }
    ESP_LOGI(TAG, "probe OK: ADS1299-family ID=0x%02x channels=%u", id.raw, (unsigned)id.channel_count);

    if (!run_internal_test()) { ESP_LOGE(TAG, "internal-test failed"); return; }
    if (!run_input_short()) { ESP_LOGE(TAG, "input-short failed"); return; }
    if (!configure_eeg250(id.channel_count)) { ESP_LOGE(TAG, "EEG250 configuration failed"); return; }
    if (!start_continuous("EEG250")) return;

    esp32p4_ads1299_frame_queue_init(&g_queue);
    g_sequence = 0u;
    if (xTaskCreate(transport_task, "ads1299_tx", 4096, NULL, 4, NULL) != pdPASS ||
        xTaskCreate(acquisition_task, "ads1299_acq", 4096, NULL, 8, NULL) != pdPASS) {
        ESP_LOGE(TAG, "task creation failed");
        (void)ads1299_stop(&g_device);
        (void)ads1299_sdatac(&g_device);
        return;
    }
    ESP_LOGI(TAG, "beginner flow complete: probe -> internal-test -> input-short -> EEG250 stream");
}
