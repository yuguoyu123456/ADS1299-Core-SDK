#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32h533_frame_queue.h"

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

static void assert_record_matches(
    const stm32h533_ads1299_frame_record_t *record,
    uint32_t sequence)
{
    size_t i;

    assert(record != NULL);
    assert(record->sequence == sequence);
    assert(record->timestamp_us == 100000u + sequence);
    assert(record->frame.status[0] == (uint8_t)(0xC0u | (sequence & 0x0Fu)));
    assert(record->frame.status[1] == (uint8_t)(sequence >> 8));
    assert(record->frame.status[2] == (uint8_t)(sequence >> 16));

    for (i = 0u; i < ADS1299_CHANNEL_COUNT; ++i) {
        assert(record->frame.channel[i] ==
               (int32_t)(sequence * 100u + (uint32_t)i));
    }
}

void test_frame_queue_fifo_wraparound_and_overflow(void)
{
    stm32h533_ads1299_frame_queue_t queue;
    stm32h533_ads1299_frame_record_t record;
    ads1299_frame_t frame;
    size_t i;
    const size_t half = STM32H533_ADS1299_FRAME_QUEUE_CAPACITY / 2u;

    stm32h533_ads1299_frame_queue_init(&queue);
    assert(stm32h533_ads1299_frame_queue_count(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_dropped(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_high_watermark(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == -1);

    for (i = 0u; i < STM32H533_ADS1299_FRAME_QUEUE_CAPACITY; ++i) {
        frame = make_frame((uint32_t)i);
        assert(stm32h533_ads1299_frame_queue_push(
                   &queue, &frame, (uint32_t)(1000u + i), (uint32_t)i) == 0);
    }

    assert(stm32h533_ads1299_frame_queue_count(&queue) ==
           STM32H533_ADS1299_FRAME_QUEUE_CAPACITY);
    assert(stm32h533_ads1299_frame_queue_high_watermark(&queue) ==
           STM32H533_ADS1299_FRAME_QUEUE_CAPACITY);

    frame = make_frame(0x55u);
    assert(stm32h533_ads1299_frame_queue_push(&queue, &frame, 9999u, 0x55u) == -1);
    assert(stm32h533_ads1299_frame_queue_dropped(&queue) == 1u);

    for (i = 0u; i < half; ++i) {
        assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert(record.sequence == (uint32_t)i);
        assert(record.timestamp_us == (uint32_t)(1000u + i));
        assert(record.frame.channel[0] == (int32_t)(i * 100u));
    }

    for (i = 0u; i < half; ++i) {
        const uint32_t sequence =
            (uint32_t)(STM32H533_ADS1299_FRAME_QUEUE_CAPACITY + i);
        frame = make_frame(sequence);
        assert(stm32h533_ads1299_frame_queue_push(
                   &queue, &frame, 2000u + sequence, sequence) == 0);
    }

    for (i = half; i < STM32H533_ADS1299_FRAME_QUEUE_CAPACITY + half; ++i) {
        assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert(record.sequence == (uint32_t)i);
        assert(record.frame.channel[7] == (int32_t)(i * 100u + 7u));
    }

    assert(stm32h533_ads1299_frame_queue_count(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_dropped(&queue) == 1u);
    assert(stm32h533_ads1299_frame_queue_high_watermark(&queue) ==
           STM32H533_ADS1299_FRAME_QUEUE_CAPACITY);
}

void test_frame_queue_argument_validation(void)
{
    stm32h533_ads1299_frame_queue_t queue;
    stm32h533_ads1299_frame_record_t record;
    ads1299_frame_t frame = make_frame(1u);

    stm32h533_ads1299_frame_queue_init(&queue);
    stm32h533_ads1299_frame_queue_init(NULL);

    assert(stm32h533_ads1299_frame_queue_push(NULL, &frame, 0u, 0u) == -1);
    assert(stm32h533_ads1299_frame_queue_push(&queue, NULL, 0u, 0u) == -1);
    assert(stm32h533_ads1299_frame_queue_pop(NULL, &record) == -1);
    assert(stm32h533_ads1299_frame_queue_pop(&queue, NULL) == -1);
    assert(stm32h533_ads1299_frame_queue_count(NULL) == 0u);
    assert(stm32h533_ads1299_frame_queue_dropped(NULL) == 0u);
    assert(stm32h533_ads1299_frame_queue_high_watermark(NULL) == 0u);
}

void test_frame_queue_long_running_interleaving(void)
{
    enum { STRESS_FRAME_COUNT = 4096 };
    stm32h533_ads1299_frame_queue_t queue;
    stm32h533_ads1299_frame_record_t record;
    uint32_t produced = 0u;
    uint32_t consumed = 0u;

    stm32h533_ads1299_frame_queue_init(&queue);

    while (produced < STRESS_FRAME_COUNT) {
        ads1299_frame_t frame;

        if (stm32h533_ads1299_frame_queue_count(&queue) ==
            STM32H533_ADS1299_FRAME_QUEUE_CAPACITY) {
            assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == 0);
            assert_record_matches(&record, consumed);
            ++consumed;
        }

        frame = make_frame(produced);
        assert(stm32h533_ads1299_frame_queue_push(
                   &queue, &frame, 100000u + produced, produced) == 0);
        ++produced;

        if ((produced % 3u) == 0u &&
            stm32h533_ads1299_frame_queue_count(&queue) != 0u) {
            assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == 0);
            assert_record_matches(&record, consumed);
            ++consumed;
        }
    }

    while (stm32h533_ads1299_frame_queue_count(&queue) != 0u) {
        assert(stm32h533_ads1299_frame_queue_pop(&queue, &record) == 0);
        assert_record_matches(&record, consumed);
        ++consumed;
    }

    assert(consumed == STRESS_FRAME_COUNT);
    assert(stm32h533_ads1299_frame_queue_count(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_dropped(&queue) == 0u);
    assert(stm32h533_ads1299_frame_queue_high_watermark(&queue) ==
           STM32H533_ADS1299_FRAME_QUEUE_CAPACITY);
}
