#ifndef ESP32C2_ADS1299_FRAME_QUEUE_H
#define ESP32C2_ADS1299_FRAME_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY
#define ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} esp32c2_ads1299_frame_record_t;

typedef struct {
    esp32c2_ads1299_frame_record_t items[ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    size_t high_watermark;
    uint32_t dropped;
} esp32c2_ads1299_frame_queue_t;

void esp32c2_ads1299_frame_queue_init(esp32c2_ads1299_frame_queue_t *queue);
bool esp32c2_ads1299_frame_queue_push(
    esp32c2_ads1299_frame_queue_t *queue,
    const ads1299_frame_t *frame,
    uint32_t timestamp_us,
    uint32_t sequence);
bool esp32c2_ads1299_frame_queue_pop(
    esp32c2_ads1299_frame_queue_t *queue,
    esp32c2_ads1299_frame_record_t *out);
size_t esp32c2_ads1299_frame_queue_count(const esp32c2_ads1299_frame_queue_t *queue);
size_t esp32c2_ads1299_frame_queue_high_watermark(const esp32c2_ads1299_frame_queue_t *queue);
uint32_t esp32c2_ads1299_frame_queue_dropped(const esp32c2_ads1299_frame_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* ESP32C2_ADS1299_FRAME_QUEUE_H */
