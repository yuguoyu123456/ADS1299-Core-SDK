#include <assert.h>
#include <stdint.h>

#include "esp32c2_frame_queue.h"

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
    esp32c2_ads1299_frame_queue_t queue;
    esp32c2_ads1299_frame_record_t record;
    esp32c2_ads1299_frame_queue_init(&queue);

    for (uint32_t n = 0; n < ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY; ++n) {
        ads1299_frame_t frame = make_frame(n);
        assert(esp32c2_ads1299_frame_queue_push(&queue, &frame, n * 4000u, n));
    }
    assert(esp32c2_ads1299_frame_queue_count(&queue) == ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(esp32c2_ads1299_frame_queue_high_watermark(&queue) == ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY);

    ads1299_frame_t overflow = make_frame(999u);
    assert(!esp32c2_ads1299_frame_queue_push(&queue, &overflow, 0u, 999u));
    assert(esp32c2_ads1299_frame_queue_dropped(&queue) == 1u);

    for (uint32_t n = 0; n < ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY / 2u; ++n) {
        assert(esp32c2_ads1299_frame_queue_pop(&queue, &record));
        assert(record.sequence == n);
        assert(record.timestamp_us == n * 4000u);
        assert(record.frame.channel[7] == (int32_t)(n * 100u + 7u));
    }

    for (uint32_t n = ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY;
         n < ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY + ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
         ++n) {
        ads1299_frame_t frame = make_frame(n);
        assert(esp32c2_ads1299_frame_queue_push(&queue, &frame, n * 4000u, n));
    }

    uint32_t expected = ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY / 2u;
    while (esp32c2_ads1299_frame_queue_pop(&queue, &record)) {
        assert(record.sequence == expected);
        assert(record.timestamp_us == expected * 4000u);
        ++expected;
    }
    assert(expected == ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY + ESP32C2_ADS1299_FRAME_QUEUE_CAPACITY / 2u);
}

void test_frame_queue_argument_validation(void)
{
    esp32c2_ads1299_frame_queue_t queue;
    esp32c2_ads1299_frame_record_t record;
    ads1299_frame_t frame = make_frame(1u);

    esp32c2_ads1299_frame_queue_init(&queue);
    esp32c2_ads1299_frame_queue_init(NULL);
    assert(!esp32c2_ads1299_frame_queue_push(NULL, &frame, 0u, 0u));
    assert(!esp32c2_ads1299_frame_queue_push(&queue, NULL, 0u, 0u));
    assert(!esp32c2_ads1299_frame_queue_pop(NULL, &record));
    assert(!esp32c2_ads1299_frame_queue_pop(&queue, NULL));
    assert(!esp32c2_ads1299_frame_queue_pop(&queue, &record));
    assert(esp32c2_ads1299_frame_queue_count(NULL) == 0u);
    assert(esp32c2_ads1299_frame_queue_high_watermark(NULL) == 0u);
    assert(esp32c2_ads1299_frame_queue_dropped(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    esp32c2_ads1299_frame_queue_t queue;
    esp32c2_ads1299_frame_record_t record;
    uint32_t produced = 0u;
    uint32_t consumed = 0u;

    esp32c2_ads1299_frame_queue_init(&queue);
    while (consumed < 8192u) {
        if (produced < 8192u && esp32c2_ads1299_frame_queue_count(&queue) < 12u) {
            ads1299_frame_t frame = make_frame(produced);
            assert(esp32c2_ads1299_frame_queue_push(
                &queue, &frame, produced * 4000u, produced));
            ++produced;
        }
        if ((produced - consumed) >= 4u || produced == 8192u) {
            assert(esp32c2_ads1299_frame_queue_pop(&queue, &record));
            assert(record.sequence == consumed);
            assert(record.timestamp_us == consumed * 4000u);
            assert(record.frame.status[2] == (uint8_t)consumed);
            assert(record.frame.channel[0] == (int32_t)(consumed * 100u));
            ++consumed;
        }
    }
    assert(produced == 8192u);
    assert(esp32c2_ads1299_frame_queue_count(&queue) == 0u);
    assert(esp32c2_ads1299_frame_queue_dropped(&queue) == 0u);
}
