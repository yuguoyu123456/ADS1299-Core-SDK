#include "esp32_frame_queue.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static esp32_ads1299_frame_item_t make_item(uint32_t sequence)
{
    esp32_ads1299_frame_item_t item;
    memset(&item, 0, sizeof(item));
    item.sequence = sequence;
    item.timestamp_us = 1000u + sequence;
    item.frame.status[0] = (uint8_t)(sequence & 0xFFu);
    for (unsigned ch = 0; ch < 8u; ++ch) {
        item.frame.channel[ch] = (int32_t)(sequence * 100u + ch);
    }
    return item;
}

static void test_fifo_and_wrap(void)
{
    esp32_ads1299_frame_queue_t q;
    esp32_ads1299_frame_queue_init(&q);

    for (uint32_t i = 0; i < ESP32_ADS1299_FRAME_QUEUE_CAPACITY; ++i) {
        const esp32_ads1299_frame_item_t item = make_item(i);
        assert(esp32_ads1299_frame_queue_push(&q, &item) == 1);
    }
    assert(esp32_ads1299_frame_queue_count(&q) == ESP32_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(esp32_ads1299_frame_queue_high_watermark(&q) == ESP32_ADS1299_FRAME_QUEUE_CAPACITY);

    for (uint32_t i = 0; i < ESP32_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++i) {
        esp32_ads1299_frame_item_t out;
        assert(esp32_ads1299_frame_queue_pop(&q, &out) == 1);
        assert(out.sequence == i);
    }
    for (uint32_t i = ESP32_ADS1299_FRAME_QUEUE_CAPACITY;
         i < ESP32_ADS1299_FRAME_QUEUE_CAPACITY + ESP32_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++i) {
        const esp32_ads1299_frame_item_t item = make_item(i);
        assert(esp32_ads1299_frame_queue_push(&q, &item) == 1);
    }

    uint32_t expected = ESP32_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
    esp32_ads1299_frame_item_t out;
    while (esp32_ads1299_frame_queue_pop(&q, &out) == 1) {
        assert(out.sequence == expected++);
    }
    assert(esp32_ads1299_frame_queue_count(&q) == 0u);
}

static void test_overflow_and_long_interleave(void)
{
    esp32_ads1299_frame_queue_t q;
    esp32_ads1299_frame_queue_init(&q);

    for (uint32_t i = 0; i < ESP32_ADS1299_FRAME_QUEUE_CAPACITY; ++i) {
        const esp32_ads1299_frame_item_t item = make_item(i);
        assert(esp32_ads1299_frame_queue_push(&q, &item) == 1);
    }
    const esp32_ads1299_frame_item_t overflow = make_item(999u);
    assert(esp32_ads1299_frame_queue_push(&q, &overflow) == 0);
    assert(esp32_ads1299_frame_queue_dropped(&q) == 1u);

    esp32_ads1299_frame_queue_init(&q);
    uint32_t consumed = 0u;
    for (uint32_t produced = 0u; produced < 8192u; ++produced) {
        const esp32_ads1299_frame_item_t item = make_item(produced);
        assert(esp32_ads1299_frame_queue_push(&q, &item) == 1);
        if ((produced & 1u) != 0u) {
            esp32_ads1299_frame_item_t out;
            assert(esp32_ads1299_frame_queue_pop(&q, &out) == 1);
            assert(out.sequence == consumed++);
            assert(out.timestamp_us == 1000u + out.sequence);
            for (unsigned ch = 0; ch < 8u; ++ch) {
                assert(out.frame.channel[ch] == (int32_t)(out.sequence * 100u + ch));
            }
        }
        if (esp32_ads1299_frame_queue_count(&q) >= ESP32_ADS1299_FRAME_QUEUE_CAPACITY - 1u) {
            esp32_ads1299_frame_item_t out;
            assert(esp32_ads1299_frame_queue_pop(&q, &out) == 1);
            assert(out.sequence == consumed++);
        }
    }
    esp32_ads1299_frame_item_t out;
    while (esp32_ads1299_frame_queue_pop(&q, &out) == 1) {
        assert(out.sequence == consumed++);
    }
    assert(consumed == 8192u);
    assert(esp32_ads1299_frame_queue_dropped(&q) == 0u);

    assert(esp32_ads1299_frame_queue_push(NULL, &overflow) == -1);
    assert(esp32_ads1299_frame_queue_push(&q, NULL) == -1);
    assert(esp32_ads1299_frame_queue_pop(NULL, &out) == -1);
    assert(esp32_ads1299_frame_queue_pop(&q, NULL) == -1);
}

int main(void)
{
    test_fifo_and_wrap();
    test_overflow_and_long_interleave();
    puts("ESP32 bounded frame queue tests: PASS");
    return 0;
}
