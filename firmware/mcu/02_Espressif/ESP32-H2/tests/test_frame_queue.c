#include <assert.h>
#include <stdint.h>
#include "esp32h2_frame_queue.h"

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
    esp32h2_ads1299_frame_queue_t q;
    esp32h2_ads1299_frame_record_t r;
    esp32h2_ads1299_frame_queue_init(&q);

    for (uint32_t n = 0; n < ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY; ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32h2_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }
    assert(esp32h2_ads1299_frame_queue_count(&q) == ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(esp32h2_ads1299_frame_queue_high_watermark(&q) == ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY);

    ads1299_frame_t overflow = make_frame(999u);
    assert(!esp32h2_ads1299_frame_queue_push(&q, &overflow, 0u, 999u));
    assert(esp32h2_ads1299_frame_queue_dropped(&q) == 1u);

    for (uint32_t n = 0; n < ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++n) {
        assert(esp32h2_ads1299_frame_queue_pop(&q, &r));
        assert(r.sequence == n);
        assert(r.timestamp_us == n * 4000u);
        assert(r.frame.status[0] == (uint8_t)(0xC0u | (n & 0x0Fu)));
        assert(r.frame.channel[7] == (int32_t)(n * 100u + 7u));
    }

    for (uint32_t n = ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY;
         n < ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY + ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
         ++n) {
        ads1299_frame_t f = make_frame(n);
        assert(esp32h2_ads1299_frame_queue_push(&q, &f, n * 4000u, n));
    }
    assert(esp32h2_ads1299_frame_queue_count(&q) == ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY);

    uint32_t expected = ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
    while (esp32h2_ads1299_frame_queue_pop(&q, &r)) {
        assert(r.sequence == expected);
        assert(r.timestamp_us == expected * 4000u);
        assert(r.frame.channel[0] == (int32_t)(expected * 100u));
        ++expected;
    }
    assert(expected == ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY + ESP32H2_ADS1299_FRAME_QUEUE_CAPACITY / 2u);
    assert(esp32h2_ads1299_frame_queue_count(&q) == 0u);
}

void test_frame_queue_argument_validation(void)
{
    esp32h2_ads1299_frame_queue_t q;
    esp32h2_ads1299_frame_record_t r;
    ads1299_frame_t f = make_frame(1u);

    esp32h2_ads1299_frame_queue_init(&q);
    assert(!esp32h2_ads1299_frame_queue_push(NULL, &f, 0u, 0u));
    assert(!esp32h2_ads1299_frame_queue_push(&q, NULL, 0u, 0u));
    assert(!esp32h2_ads1299_frame_queue_pop(NULL, &r));
    assert(!esp32h2_ads1299_frame_queue_pop(&q, NULL));
    assert(esp32h2_ads1299_frame_queue_count(NULL) == 0u);
    assert(esp32h2_ads1299_frame_queue_high_watermark(NULL) == 0u);
    assert(esp32h2_ads1299_frame_queue_dropped(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    esp32h2_ads1299_frame_queue_t q;
    esp32h2_ads1299_frame_record_t r;
    uint32_t produced = 0u;
    uint32_t consumed = 0u;

    esp32h2_ads1299_frame_queue_init(&q);
    while (consumed < 8192u) {
        if (produced < 8192u && esp32h2_ads1299_frame_queue_count(&q) < 12u) {
            ads1299_frame_t f = make_frame(produced);
            assert(esp32h2_ads1299_frame_queue_push(&q, &f, produced * 4000u, produced));
            ++produced;
        }

        if ((produced - consumed) >= 4u || produced == 8192u) {
            assert(esp32h2_ads1299_frame_queue_pop(&q, &r));
            assert(r.sequence == consumed);
            assert(r.timestamp_us == consumed * 4000u);
            assert(r.frame.status[1] == (uint8_t)(consumed >> 8));
            assert(r.frame.status[2] == (uint8_t)consumed);
            for (unsigned i = 0; i < 8u; ++i) {
                assert(r.frame.channel[i] == (int32_t)(consumed * 100u + i));
            }
            ++consumed;
        }
    }

    assert(produced == 8192u);
    assert(esp32h2_ads1299_frame_queue_count(&q) == 0u);
    assert(esp32h2_ads1299_frame_queue_dropped(&q) == 0u);
    assert(esp32h2_ads1299_frame_queue_high_watermark(&q) >= 4u);
    assert(esp32h2_ads1299_frame_queue_high_watermark(&q) <= 12u);
}
