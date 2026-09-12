#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "ads1299_stream_queue.h"

static esp32s3_ads1299_stream_item_t make_item(uint32_t sequence) {
    esp32s3_ads1299_stream_item_t item = {0};
    item.sequence = sequence;
    item.timestamp_us = (uint64_t)sequence * 4000u;
    item.frame.status[0] = 0xC0u;
    item.frame.status[1] = (uint8_t)sequence;
    item.frame.status[2] = (uint8_t)(sequence >> 8);
    for (size_t channel = 0; channel < 8u; ++channel) {
        item.frame.channel[channel] = (int32_t)(sequence * 16u + channel);
    }
    return item;
}

static void assert_item(uint32_t sequence,
                        const esp32s3_ads1299_stream_item_t *item) {
    assert(item->sequence == sequence);
    assert(item->timestamp_us == (uint64_t)sequence * 4000u);
    assert(item->frame.status[0] == 0xC0u);
    assert(item->frame.status[1] == (uint8_t)sequence);
    assert(item->frame.status[2] == (uint8_t)(sequence >> 8));
    for (size_t channel = 0; channel < 8u; ++channel) {
        assert(item->frame.channel[channel] == (int32_t)(sequence * 16u + channel));
    }
}

static void test_fifo_and_overflow(void) {
    esp32s3_ads1299_stream_queue_t queue;
    esp32s3_ads1299_stream_queue_init(&queue);

    assert(esp32s3_ads1299_stream_queue_count(&queue) == 0u);
    for (uint32_t i = 0; i < ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY; ++i) {
        const esp32s3_ads1299_stream_item_t item = make_item(i);
        assert(esp32s3_ads1299_stream_queue_push(&queue, &item) == 1);
    }
    assert(esp32s3_ads1299_stream_queue_count(&queue) ==
           ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY);
    assert(esp32s3_ads1299_stream_queue_high_watermark(&queue) ==
           ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY);

    const esp32s3_ads1299_stream_item_t overflow = make_item(999u);
    assert(esp32s3_ads1299_stream_queue_push(&queue, &overflow) == 0);
    assert(esp32s3_ads1299_stream_queue_dropped(&queue) == 1u);

    for (uint32_t i = 0; i < ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY; ++i) {
        esp32s3_ads1299_stream_item_t item;
        assert(esp32s3_ads1299_stream_queue_pop(&queue, &item) == 1);
        assert_item(i, &item);
    }
    assert(esp32s3_ads1299_stream_queue_count(&queue) == 0u);
}

static void test_wraparound_and_interleaving(void) {
    esp32s3_ads1299_stream_queue_t queue;
    esp32s3_ads1299_stream_queue_init(&queue);

    uint32_t produced = 0u;
    uint32_t consumed = 0u;
    while (consumed < 8192u) {
        while (produced < 8192u &&
               esp32s3_ads1299_stream_queue_count(&queue) <
                   ESP32S3_ADS1299_STREAM_QUEUE_CAPACITY - 1u) {
            const esp32s3_ads1299_stream_item_t item = make_item(produced);
            assert(esp32s3_ads1299_stream_queue_push(&queue, &item) == 1);
            produced++;
        }

        esp32s3_ads1299_stream_item_t item;
        assert(esp32s3_ads1299_stream_queue_pop(&queue, &item) == 1);
        assert_item(consumed, &item);
        consumed++;
    }

    assert(produced == 8192u);
    assert(esp32s3_ads1299_stream_queue_count(&queue) == 0u);
    assert(esp32s3_ads1299_stream_queue_dropped(&queue) == 0u);
}

static void test_invalid_arguments(void) {
    esp32s3_ads1299_stream_queue_t queue;
    esp32s3_ads1299_stream_item_t item = make_item(1u);
    esp32s3_ads1299_stream_queue_init(&queue);

    assert(esp32s3_ads1299_stream_queue_push(NULL, &item) == -1);
    assert(esp32s3_ads1299_stream_queue_push(&queue, NULL) == -1);
    assert(esp32s3_ads1299_stream_queue_pop(NULL, &item) == -1);
    assert(esp32s3_ads1299_stream_queue_pop(&queue, NULL) == -1);
    assert(esp32s3_ads1299_stream_queue_count(NULL) == 0u);
    assert(esp32s3_ads1299_stream_queue_high_watermark(NULL) == 0u);
    assert(esp32s3_ads1299_stream_queue_dropped(NULL) == 0u);
}

int main(void) {
    test_fifo_and_overflow();
    test_wraparound_and_interleaving();
    test_invalid_arguments();
    puts("ESP32-S3 bounded stream queue tests: PASS");
    return 0;
}
