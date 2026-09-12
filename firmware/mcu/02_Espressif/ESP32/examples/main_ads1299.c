/* Progressive classic-ESP32 integration example.
 * All ADS1299 register/command semantics remain in the shared core. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "esp32_frame_queue.h"
#include "esp32_devkitc_v4_ads1299.h"

#include <inttypes.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Compatibility debugger snapshot. Odd sequence means a write is in progress. */
volatile ads1299_frame_t ads1299_latest_frame;
volatile uint32_t ads1299_frame_sequence;

static const char *TAG = "ads1299_esp32";
static ads1299_mcu_port_t s_mcu;
static ads1299_t s_device;
static ads1299_frame_queue_t s_queue;
static portMUX_TYPE s_queue_lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_stream_sequence;

static int wait_drdy(uint32_t timeout_ms)
{
    const int64_t deadline = esp_timer_get_time() + (int64_t)timeout_ms * 1000LL;
    for (;;) {
        const int ready = s_mcu.hal.drdy_read(s_mcu.hal.user);
        if (ready < 0) return -1;
        if (ready == 0) return 1;
        if (esp_timer_get_time() >= deadline) return 0;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static int capture_frames(const char *label, unsigned count)
{
    for (unsigned i = 0; i < count; ++i) {
        const int ready = wait_drdy(ADS1299_BOARD_DRDY_TIMEOUT_MS);
        if (ready <= 0) {
            ESP_LOGE(TAG, "%s: DRDY %s at frame %u", label,
                     ready == 0 ? "timeout" : "read failure", i);
            return -1;
        }
        ads1299_frame_t frame;
        if (ads1299_read_frame_continuous(&s_device, &frame) != ADS1299_OK) {
            ESP_LOGE(TAG, "%s: frame read failed at %u", label, i);
            return -1;
        }
    }
    ESP_LOGI(TAG, "%s: %u frames captured", label, count);
    return 0;
}

static int run_finite_profile(const char *label, unsigned count)
{
    if (ads1299_rdatac(&s_device) != ADS1299_OK) return -1;
    if (ads1299_start(&s_device) != ADS1299_OK) return -1;
    const int result = capture_frames(label, count);
    const ads1299_status_t stop_status = ads1299_stop(&s_device);
    const ads1299_status_t sdatac_status = ads1299_sdatac(&s_device);
    return result == 0 && stop_status == ADS1299_OK && sdatac_status == ADS1299_OK ? 0 : -1;
}

static int queue_push(const esp32_ads1299_frame_item_t *item)
{
    int result;
    portENTER_CRITICAL(&s_queue_lock);
    result = esp32_ads1299_frame_queue_push(&s_queue, item);
    portEXIT_CRITICAL(&s_queue_lock);
    return result;
}

static int queue_pop(esp32_ads1299_frame_item_t *item)
{
    int result;
    portENTER_CRITICAL(&s_queue_lock);
    result = esp32_ads1299_frame_queue_pop(&s_queue, item);
    portEXIT_CRITICAL(&s_queue_lock);
    return result;
}

static void acquisition_task(void *arg)
{
    (void)arg;
    for (;;) {
        const int ready = wait_drdy(ADS1299_BOARD_DRDY_TIMEOUT_MS);
        if (ready == 0) {
            ESP_LOGW(TAG, "stream: DRDY timeout");
            continue;
        }
        if (ready < 0) {
            ESP_LOGE(TAG, "stream: DRDY read failure");
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        esp32_ads1299_frame_item_t item = {
            .timestamp_us = (uint64_t)esp_timer_get_time(),
            .sequence = s_stream_sequence++,
        };
        if (ads1299_read_frame_continuous(&s_device, &item.frame) != ADS1299_OK) {
            ESP_LOGE(TAG, "stream: frame read failure");
            continue;
        }

        ++ads1299_frame_sequence;
        ads1299_latest_frame = item.frame;
        ++ads1299_frame_sequence;

        if (queue_push(&item) == 0) {
            uint32_t dropped;
            size_t high;
            portENTER_CRITICAL(&s_queue_lock);
            dropped = esp32_ads1299_frame_queue_dropped(&s_queue);
            high = esp32_ads1299_frame_queue_high_watermark(&s_queue);
            portEXIT_CRITICAL(&s_queue_lock);
            ESP_LOGW(TAG, "stream: queue overflow dropped=%" PRIu32 " high=%u/%u",
                     dropped, (unsigned)high,
                     (unsigned)ESP32_ADS1299_FRAME_QUEUE_CAPACITY);
        }
    }
}

static void transport_task(void *arg)
{
    (void)arg;
    uint32_t last_report = 0u;
    for (;;) {
        esp32_ads1299_frame_item_t item;
        if (queue_pop(&item) == 1) {
            /* Replace this sparse log with UART/Wi-Fi/BLE transport. Keep all
             * potentially blocking transport work out of acquisition_task. */
            if ((item.sequence - last_report) >= 250u) {
                size_t depth;
                size_t high;
                uint32_t dropped;
                portENTER_CRITICAL(&s_queue_lock);
                depth = esp32_ads1299_frame_queue_count(&s_queue);
                high = esp32_ads1299_frame_queue_high_watermark(&s_queue);
                dropped = esp32_ads1299_frame_queue_dropped(&s_queue);
                portEXIT_CRITICAL(&s_queue_lock);
                ESP_LOGI(TAG,
                         "EEG250 seq=%" PRIu32 " ts=%" PRIu64
                         " queue=%u high=%u dropped=%" PRIu32,
                         item.sequence, item.timestamp_us, (unsigned)depth,
                         (unsigned)high, dropped);
                last_report = item.sequence;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

static int configure_eeg_250(void)
{
    if (ads1299_set_data_rate(&s_device, ADS1299_DR_250SPS) != ADS1299_OK) return -1;
    if (ads1299_set_srb1(&s_device, 0) != ADS1299_OK) return -1;
    const uint8_t channels = s_device.channel_count == 0u ? 8u : s_device.channel_count;
    for (uint8_t channel = 1u; channel <= channels; ++channel) {
        if (ads1299_set_channel(&s_device, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) return -1;
    }
    ESP_LOGI(TAG, "EEG profile: 250 SPS, gain 24, normal input, SRB1/SRB2 off");
    return 0;
}

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_device_id_t identity;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&s_mcu, &hal) != 0) return 2;
    ads1299_port_t port = ads1299_mcu_make_port(&s_mcu);
    if (ads1299_init(&s_device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&s_device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&s_device) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&s_device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 6;
    ESP_LOGI(TAG, "probe: ADS1299 family detected, channels=%u",
             (unsigned)s_device.channel_count);

    if (ads1299_set_data_rate(&s_device, ADS1299_DR_250SPS) != ADS1299_OK) return 7;
    if (ads1299_configure_internal_test(&s_device, ADS1299_GAIN_24, 0,
            ADS1299_TEST_FREQ_FCLK_DIV_2_21) != ADS1299_OK) return 8;
    if (run_finite_profile("internal-test", 8u) != 0) return 9;

    if (ads1299_configure_input_short_test(&s_device, ADS1299_GAIN_24) != ADS1299_OK) return 10;
    if (run_finite_profile("input-short", 8u) != 0) return 11;

    if (configure_eeg_250() != 0) return 12;
    esp32_ads1299_frame_queue_init(&s_queue);

    if (xTaskCreate(acquisition_task, "ads1299_acq", 4096, NULL,
                    configMAX_PRIORITIES - 2, NULL) != pdPASS) return 13;
    if (xTaskCreate(transport_task, "ads1299_transport", 4096, NULL,
                    tskIDLE_PRIORITY + 2, NULL) != pdPASS) return 14;

    if (ads1299_rdatac(&s_device) != ADS1299_OK) return 15;
    if (ads1299_start(&s_device) != ADS1299_OK) return 16;
    ESP_LOGI(TAG, "stream: bounded EEG250 acquisition started");

    for (;;) vTaskDelay(pdMS_TO_TICKS(10000));
}
