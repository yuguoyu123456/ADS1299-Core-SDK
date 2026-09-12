#ifndef ESP32P4_ADS1299_FRAME_QUEUE_H
#define ESP32P4_ADS1299_FRAME_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ads1299.h"

#ifndef ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY
#define ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY 32u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint32_t timestamp_us;
    uint32_t sequence;
} esp32p4_ads1299_frame_record_t;

typedef struct {
    esp32p4_ads1299_frame_record_t items[ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    size_t high_watermark;
    uint32_t dropped;
} esp32p4_ads1299_frame_queue_t;

void esp32p4_ads1299_frame_queue_init(esp32p4_ads1299_frame_queue_t *q);
bool esp32p4_ads1299_frame_queue_push(esp32p4_ads1299_frame_queue_t *q,
                                       const ads1299_frame_t *frame,
                                       uint32_t timestamp_us,
                                       uint32_t sequence);
bool esp32p4_ads1299_frame_queue_pop(esp32p4_ads1299_frame_queue_t *q,
                                      esp32p4_ads1299_frame_record_t *out);
size_t esp32p4_ads1299_frame_queue_count(const esp32p4_ads1299_frame_queue_t *q);
size_t esp32p4_ads1299_frame_queue_high_watermark(const esp32p4_ads1299_frame_queue_t *q);
uint32_t esp32p4_ads1299_frame_queue_dropped(const esp32p4_ads1299_frame_queue_t *q);

#endif
