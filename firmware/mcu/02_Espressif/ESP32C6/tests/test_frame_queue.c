#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299.h"
#include "esp32c6_frame_queue.h"

static ads1299_frame_t make_frame(uint32_t n)
{
    ads1299_frame_t f;
    memset(&f, 0, sizeof(f));
    f.status[0] = (uint8_t)(n >> 16);
    f.status[1] = (uint8_t)(n >> 8);
    f.status[2] = (uint8_t)n;
    for (size_t i = 0; i < 8u; ++i) f.channel[i] = (int32_t)(n * 16u + (uint32_t)i);
    return f;
}

static void test_fifo_and_wrap(void)
{
    esp32c6_ads1299_frame_queue_t q;
    esp32c6_ads1299_frame_queue_init(&q);
    for (uint32_t i = 0; i < ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY; ++i) {
        ads1299_frame_t f = make_frame(i);
        assert(esp32c6_ads1299_frame_queue_push(&q, &f, i + 100u, i));
    }
    assert(esp32c6_ads1299_frame_queue_count(&q) == ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(esp32c6_ads1299_frame_queue_high_watermark(&q) == ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY);
    for (uint32_t i = 0; i < ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++i) {
        esp32c6_ads1299_frame_record_t r;
        assert(esp32c6_ads1299_frame_queue_pop(&q, &r));
        assert(r.sequence == i);
        assert(r.timestamp_us == i + 100u);
    }
    for (uint32_t i = ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY;
         i < ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY + ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++i) {
        ads1299_frame_t f = make_frame(i);
        assert(esp32c6_ads1299_frame_queue_push(&q, &f, i + 100u, i));
    }
    uint32_t expected = ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
    esp32c6_ads1299_frame_record_t r;
    while (esp32c6_ads1299_frame_queue_pop(&q, &r)) {
        assert(r.sequence == expected);
        assert(r.timestamp_us == expected + 100u);
        for (size_t ch = 0; ch < 8u; ++ch) assert(r.frame.channel[ch] == (int32_t)(expected * 16u + ch));
        ++expected;
    }
}

static void test_overflow_and_invalid_args(void)
{
    esp32c6_ads1299_frame_queue_t q;
    esp32c6_ads1299_frame_queue_init(&q);
    ads1299_frame_t f = make_frame(1u);
    for (size_t i = 0; i < ESP32C6_ADS1299_FRAME_QUEUE_CAPACITY; ++i)
        assert(esp32c6_ads1299_frame_queue_push(&q, &f, 0u, (uint32_t)i));
    assert(!esp32c6_ads1299_frame_queue_push(&q, &f, 0u, 99u));
    assert(esp32c6_ads1299_frame_queue_dropped(&q) == 1u);
    assert(!esp32c6_ads1299_frame_queue_push(NULL, &f, 0u, 0u));
    assert(!esp32c6_ads1299_frame_queue_push(&q, NULL, 0u, 0u));
    assert(!esp32c6_ads1299_frame_queue_pop(NULL, NULL));
}

static void test_long_interleave(void)
{
    esp32c6_ads1299_frame_queue_t q;
    esp32c6_ads1299_frame_queue_init(&q);
    uint32_t next_out = 0u;
    for (uint32_t i = 0; i < 8192u; ++i) {
        ads1299_frame_t f = make_frame(i);
        while (!esp32c6_ads1299_frame_queue_push(&q, &f, i * 4000u, i)) {
            esp32c6_ads1299_frame_record_t r;
            assert(esp32c6_ads1299_frame_queue_pop(&q, &r));
            assert(r.sequence == next_out++);
        }
        if ((i & 1u) != 0u) {
            esp32c6_ads1299_frame_record_t r;
            assert(esp32c6_ads1299_frame_queue_pop(&q, &r));
            assert(r.sequence == next_out++);
        }
    }
    esp32c6_ads1299_frame_record_t r;
    while (esp32c6_ads1299_frame_queue_pop(&q, &r)) assert(r.sequence == next_out++);
    assert(next_out == 8192u);
}

void esp32c6_test_frame_queue(void)
{
    test_fifo_and_wrap();
    test_overflow_and_invalid_args();
    test_long_interleave();
}
