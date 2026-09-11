#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32f072_frame_queue.h"

static ads1299_frame_t make_frame(uint32_t marker)
{
    ads1299_frame_t frame = {0};
    size_t i;

    frame.status[0] = (uint8_t)(0xC0u | (marker & 0x0Fu));
    frame.status[1] = (uint8_t)(marker >> 8);
    frame.status[2] = (uint8_t)(marker >> 16);
    for (i = 0u; i < ADS1299_CHANNEL_COUNT; ++i) {
        frame.channel[i] = (int32_t)(marker * 100u + (uint32_t)i);
    }
    return frame;
}

static void assert_record(const stm32f072_ads1299_frame_record_t *record,
                          uint32_t sequence,
                          uint32_t timestamp_base)
{
    size_t i;

    assert(record != NULL);
    assert(record->sequence == sequence);
    assert(record->timestamp_us == timestamp_base + sequence);
    assert(record->frame.status[0] == (uint8_t)(0xC0u | (sequence & 0x0Fu)));
    assert(record->frame.status[1] == (uint8_t)(sequence >> 8));
    assert(record->frame.status[2] == (uint8_t)(sequence >> 16));
    for (i = 0u; i < ADS1299_CHANNEL_COUNT; ++i) {
        assert(record->frame.channel[i] == (int32_t)(sequence * 100u + (uint32_t)i));
    }
}

void test_frame_queue_fifo_wraparound_and_overflow(void)
{
    stm32f072_ads1299_frame_queue_t queue;
    stm32f072_ads1299_frame_record_t record;
    ads1299_frame_t frame;
    size_t i;
    const size_t half = STM32F072_ADS1299_FRAME_QUEUE_CAPACITY / 2u;

    stm32f072_ads1299_frame_queue_init(&queue);
    for (i = 0u; i < STM32F072_ADS1299_FRAME_QUEUE_CAPACITY; ++i) {
        frame = make_frame((uint32_t)i);
        assert(stm32f072_ads1299_frame_queue_push(&queue, &frame,
                                                   1000u + (uint32_t)i,
                                                   (uint32_t)i) == 0);
    }
    assert(stm32f072_ads1299_frame_queue_high_watermark(&queue) ==
           STM32F072_ADS1299_FRAME_QUEUE_CAPACITY);

    frame = make_frame(0x55u);
    assert(stm32f072_ads1299_frame_queue_push(&queue, &frame, 9999u, 0x55u) == -1);
    assert(stm32f072_ads1299_frame_queue_dropped(&queue) == 1u);

    for (i = 0u; i < half; ++i) {
        assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert(record.sequence == (uint32_t)i);
    }
    for (i = 0u; i < half; ++i) {
        const uint32_t seq = (uint32_t)(STM32F072_ADS1299_FRAME_QUEUE_CAPACITY + i);
        frame = make_frame(seq);
        assert(stm32f072_ads1299_frame_queue_push(&queue, &frame, 2000u + seq, seq) == 0);
    }
    for (i = half; i < STM32F072_ADS1299_FRAME_QUEUE_CAPACITY + half; ++i) {
        assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert(record.sequence == (uint32_t)i);
    }
    assert(stm32f072_ads1299_frame_queue_count(&queue) == 0u);
}

void test_frame_queue_argument_validation(void)
{
    stm32f072_ads1299_frame_queue_t queue;
    stm32f072_ads1299_frame_record_t record;
    ads1299_frame_t frame = make_frame(1u);

    stm32f072_ads1299_frame_queue_init(&queue);
    stm32f072_ads1299_frame_queue_init(NULL);
    assert(stm32f072_ads1299_frame_queue_push(NULL, &frame, 0u, 0u) == -1);
    assert(stm32f072_ads1299_frame_queue_push(&queue, NULL, 0u, 0u) == -1);
    assert(stm32f072_ads1299_frame_queue_pop(NULL, &record) == -1);
    assert(stm32f072_ads1299_frame_queue_pop(&queue, NULL) == -1);
    assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == -1);
    assert(stm32f072_ads1299_frame_queue_count(NULL) == 0u);
    assert(stm32f072_ads1299_frame_queue_dropped(NULL) == 0u);
    assert(stm32f072_ads1299_frame_queue_high_watermark(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    enum { STRESS_FRAME_COUNT = 2048 };
    stm32f072_ads1299_frame_queue_t queue;
    stm32f072_ads1299_frame_record_t record;
    uint32_t produced = 0u;
    uint32_t consumed = 0u;

    stm32f072_ads1299_frame_queue_init(&queue);
    while (produced < STRESS_FRAME_COUNT) {
        ads1299_frame_t frame;

        if (stm32f072_ads1299_frame_queue_count(&queue) == STM32F072_ADS1299_FRAME_QUEUE_CAPACITY) {
            assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == 0);
            assert_record(&record, consumed++, 100000u);
        }

        frame = make_frame(produced);
        assert(stm32f072_ads1299_frame_queue_push(&queue, &frame,
                                                   100000u + produced,
                                                   produced) == 0);
        ++produced;

        if ((produced % 2u) == 0u && stm32f072_ads1299_frame_queue_count(&queue) != 0u) {
            assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == 0);
            assert_record(&record, consumed++, 100000u);
        }
    }

    while (stm32f072_ads1299_frame_queue_count(&queue) != 0u) {
        assert(stm32f072_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert_record(&record, consumed++, 100000u);
    }

    assert(consumed == STRESS_FRAME_COUNT);
    assert(stm32f072_ads1299_frame_queue_dropped(&queue) == 0u);
    assert(stm32f072_ads1299_frame_queue_high_watermark(&queue) ==
           STM32F072_ADS1299_FRAME_QUEUE_CAPACITY);
}
