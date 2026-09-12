#include <assert.h>
#include <stdint.h>
#include "esp32p4_frame_queue.h"

static ads1299_frame_t make_frame(uint32_t n)
{
    ads1299_frame_t frame = {0};
    frame.status[0] = (uint8_t)(0xC0u | (n & 0x0Fu));
    frame.status[1] = (uint8_t)(n >> 8);
    frame.status[2] = (uint8_t)n;
    for (unsigned i = 0; i < 8u; ++i) {
        frame.channel[i] = (int32_t)(n * 100u + i);
    }
    return frame;
}

void test_frame_queue_fifo_wraparound_and_overflow(void)
{
    esp32p4_ads1299_frame_queue_t q;
    esp32p4_ads1299_frame_record_t r;
    esp32p4_ads1299_frame_queue_init(&q);

    for (uint32_t n = 0; n < ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY; ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32p4_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }
    assert(esp32p4_ads1299_frame_queue_count(&q) == ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(esp32p4_ads1299_frame_queue_high_watermark(&q) == ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY);

    ads1299_frame_t overflow = make_frame(999u);
    assert(!esp32p4_ads1299_frame_queue_push(&q, &overflow, 0u, 999u));
    assert(esp32p4_ads1299_frame_queue_dropped(&q) == 1u);

    for (uint32_t n = 0; n < ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++n) {
        assert(esp32p4_ads1299_frame_queue_pop(&q, &r));
        assert(r.sequence == n);
        assert(r.timestamp_us == n * 4000u);
        assert(r.frame.status[0] == (uint8_t)(0xC0u | (n & 0x0Fu)));
        assert(r.frame.channel[7] == (int32_t)(n * 100u + 7u));
    }

    for (uint32_t n = ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY;
         n < ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY + ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
         ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32p4_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }

    uint32_t expected = ESP32P4_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
    while (esp32p4_ads1299_frame_queue_pop(&q, &r)) {
        assert(r.sequence == expected);
        assert(r.frame.channel[0] == (int32_t)(expected * 100u));
        ++expected;
    }
    assert(esp32p4_ads1299_frame_queue_count(&q) == 0u);
}

void test_frame_queue_argument_validation(void)
{
    esp32p4_ads1299_frame_queue_t q;
    esp32p4_ads1299_frame_record_t r;
    ads1299_frame_t f = make_frame(1u);
    esp32p4_ads1299_frame_queue_init(&q);
    assert(!esp32p4_ads1299_frame_queue_push(NULL, &f, 0u, 0u));
    assert(!esp32p4_ads1299_frame_queue_push(&q, NULL, 0u, 0u));
    assert(!esp32p4_ads1299_frame_queue_pop(NULL, &r));
    assert(!esp32p4_ads1299_frame_queue_pop(&q, NULL));
    assert(esp32p4_ads1299_frame_queue_count(NULL) == 0u);
    assert(esp32p4_ads1299_frame_queue_high_watermark(NULL) == 0u);
    assert(esp32p4_ads1299_frame_queue_dropped(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    esp32p4_ads1299_frame_queue_t q;
    esp32p4_ads1299_frame_record_t r;
    uint32_t produced = 0u;
    uint32_t consumed = 0u;
    esp32p4_ads1299_frame_queue_init(&q);

    while (consumed < 16384u) {
        if (produced < 16384u && esp32p4_ads1299_frame_queue_count(&q) < 24u) {
            ads1299_frame_t f = make_frame(produced);
            assert(esp32p4_ads1299_frame_queue_push(&q, &f, produced * 4000u, produced));
            ++produced;
        }
        if ((produced - consumed) >= 8u || produced == 16384u) {
            assert(esp32p4_ads1299_frame_queue_pop(&q, &r));
            assert(r.sequence == consumed);
            assert(r.timestamp_us == consumed * 4000u);
            for (unsigned i = 0; i < 8u; ++i) {
                assert(r.frame.channel[i] == (int32_t)(consumed * 100u + i));
            }
            ++consumed;
        }
    }

    assert(produced == 16384u);
    assert(esp32p4_ads1299_frame_queue_count(&q) == 0u);
    assert(esp32p4_ads1299_frame_queue_dropped(&q) == 0u);
    assert(esp32p4_ads1299_frame_queue_high_watermark(&q) >= 8u);
    assert(esp32p4_ads1299_frame_queue_high_watermark(&q) <= 24u);
}
