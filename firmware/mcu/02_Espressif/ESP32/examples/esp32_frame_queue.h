#ifndef ESP32_ADS1299_FRAME_QUEUE_H
#define ESP32_ADS1299_FRAME_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299.h"

#ifndef ESP32_ADS1299_FRAME_QUEUE_CAPACITY
#define ESP32_ADS1299_FRAME_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint64_t timestamp_us;
    uint32_t sequence;
} esp32_ads1299_frame_item_t;

typedef struct {
    esp32_ads1299_frame_item_t items[ESP32_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    size_t high_watermark;
    uint32_t dropped;
} esp32_ads1299_frame_queue_t;

void esp32_ads1299_frame_queue_init(esp32_ads1299_frame_queue_t *q);
int esp32_ads1299_frame_queue_push(esp32_ads1299_frame_queue_t *q,
                                   const esp32_ads1299_frame_item_t *item);
int esp32_ads1299_frame_queue_pop(esp32_ads1299_frame_queue_t *q,
                                  esp32_ads1299_frame_item_t *item);
size_t esp32_ads1299_frame_queue_count(const esp32_ads1299_frame_queue_t *q);
size_t esp32_ads1299_frame_queue_high_watermark(const esp32_ads1299_frame_queue_t *q);
uint32_t esp32_ads1299_frame_queue_dropped(const esp32_ads1299_frame_queue_t *q);

#endif
