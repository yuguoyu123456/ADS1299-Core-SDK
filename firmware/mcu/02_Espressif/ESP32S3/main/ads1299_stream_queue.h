#ifndef ESP32S3_ADS1299_STREAM_QUEUE_H
#define ESP32S3_ADS1299_STREAM_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

#ifndef ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY
#define ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY 16u
#endif

typedef struct {
    ads1299_frame_t frame;
    uint64_t timestamp_us;
    uint32_t sequence;
} esp32s3_ads1299_stream_item_t;

typedef struct {
    esp32s3_ads1299_stream_item_t items[ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    size_t high_watermark;
    uint32_t dropped;
} esp32s3_ads1299_stream_queue_t;

void esp32s3_ads1299_stream_queue_init(esp32s3_ads1299_stream_queue_t *queue);
int esp32s3_ads1299_stream_queue_push(
    esp32s3_ads1299_stream_queue_t *queue,
    const esp32s3_ads1299_stream_item_t *item);
int esp32s3_ads1299_stream_queue_pop(
    esp32s3_ads1299_stream_queue_t *queue,
    esp32s3_ads1299_stream_item_t *item);
size_t esp32s3_ads1299_stream_queue_count(
    const esp32s3_ads1299_stream_queue_t *queue);
size_t esp32s3_ads1299_stream_queue_high_watermark(
    const esp32s3_ads1299_stream_queue_t *queue);
uint32_t esp32s3_ads1299_stream_queue_dropped(
    const esp32s3_ads1299_stream_queue_t *queue);

#endif /* ESP32S3_ADS1299_STREAM_QUEUE_H */
