#include <assert.h>
#include <stdint.h>
#include "esp32c5_frame_queue.h"

static ads1299_frame_t make_frame(uint32_t n)
{
    ads1299_frame_t frame = {0};
    frame.status[0] = (uint8_t)(0xC0u | (n & 0x0Fu));
    frame.status[1] = (uint8_t)(n >> 8);
    frame.status[2] = (uint8_t)n;
    for (unsigned i = 0; i < 8u; ++i) frame.channel[i] = (int32_t)(n * 100u + i);
    return frame;
}

void test_frame_queue_fifo_wraparound_and_overflow(void)
{
    esp32c5_ads1299_frame_queue_t q;
    esp32c5_ads1299_frame_record_t r;
    esp32c5_ads1299_frame_queue_init(&q);
    for (uint32_t n = 0; n < ESP32C5_ADS1299_FRAME_QUEUE_CAPACITY; ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32c5_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }
    assert(esp32c5_ads1299_frame_queue_high_watermark(&q) == ESP32C5_ADS1299_FRAME_QUEUE_CAPACITY);
    ads1299_frame_t overflow = make_frame(999u);
    assert(!esp32c5_ads1299_frame_queue_push(&q, &overflow, 0u, 999u));
    assert(esp32c5_ads1299_frame_queue_dropped(&q) == 1u);
    for (uint32_t n = 0; n < ESP32C5_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++n) {
        assert(esp32c5_ads1299_frame_queue_pop(&q, &r));
        assert(r.sequence == n && r.timestamp_us == n * 4000u);
    }
    for (uint32_t n = ESP32C5_ADS1299_FRAME_QUEUE_CAPACITY; n < 24u; ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32c5_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }
}

void test_frame_queue_argument_validation(void)
{
    esp32c5_ads1299_frame_queue_t q;
    esp32c5_ads1299_frame_record_t r;
    ads1299_frame_t f = make_frame(1u);
    esp32c5_ads1299_frame_queue_init(&q);
    assert(!esp32c5_ads1299_frame_queue_push(NULL, &f, 0u, 0u));
    assert(!esp32c5_ads1299_frame_queue_push(&q, NULL, 0u, 0u));
    assert(!esp32c5_ads1299_frame_queue_pop(NULL, &r));
    assert(!esp32c5_ads1299_frame_queue_pop(&q, NULL));
    assert(esp32c5_ads1299_frame_queue_count(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    esp32c5_ads1299_frame_queue_t q;
    esp32c5_ads1299_frame_record_t r;
    uint32_t produced = 0u, consumed = 0u;
    esp32c5_ads1299_frame_queue_init(&q);
    while (consumed < 8192u) {
        if (produced < 8192u && esp32c5_ads1299_frame_queue_count(&q) < 12u) {
            ads1299_frame_t f = make_frame(produced);
            assert(esp32c5_ads1299_frame_queue_push(&q, &f, produced * 4000u, produced));
            ++produced;
        }
        if ((produced - consumed) >= 4u || produced == 8192u) {
            assert(esp32c5_ads1299_frame_queue_pop(&q, &r));
            assert(r.sequence == consumed);
            assert(r.timestamp_us == consumed * 4000u);
            assert(r.frame.channel[0] == (int32_t)(consumed * 100u));
            ++consumed;
        }
    }
    assert(esp32c5_ads1299_frame_queue_dropped(&q) == 0u);
}
