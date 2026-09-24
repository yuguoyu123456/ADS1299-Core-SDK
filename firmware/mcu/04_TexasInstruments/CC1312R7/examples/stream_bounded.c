/* CC1312R7 bounded streaming pattern for ADS1299.
 *
 * Acquisition and transport are deliberately separated: the DRDY path only
 * captures a frame and enqueues it. A radio/UART/network task may dequeue and
 * transmit later. This prevents slow transport from blocking 250-SPS capture.
 *
 * This file is a platform example, not a shared-core transport policy.
 */
#include <stdint.h>
#include <stddef.h>

#include "ads1299.h"
#include "ads1299_spi.h"
#include "board_ads1299_binding.h"

#define STREAM_QUEUE_DEPTH 16u
#define STREAM_CAPTURE_FRAMES 250u

typedef struct {
    ads1299_frame_t slots[STREAM_QUEUE_DEPTH];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
    volatile uint32_t dropped;
} frame_queue_t;

static int queue_push(frame_queue_t *q, const ads1299_frame_t *frame)
{
    if (!q || !frame) return -1;
    if (q->count >= STREAM_QUEUE_DEPTH) {
        q->dropped++;
        return 1; /* Explicit overflow: newest frame is dropped. */
    }
    q->slots[q->head] = *frame;
    q->head = (uint8_t)((q->head + 1u) % STREAM_QUEUE_DEPTH);
    q->count++;
    return 0;
}

static int queue_pop(frame_queue_t *q, ads1299_frame_t *frame)
{
    if (!q || !frame) return -1;
    if (q->count == 0u) return 1;
    *frame = q->slots[q->tail];
    q->tail = (uint8_t)((q->tail + 1u) % STREAM_QUEUE_DEPTH);
    q->count--;
    return 0;
}

static int clean_stop(ads1299_t *device)
{
    int rc = 0;
    if (ads1299_stop(device) != ADS1299_OK) rc = 1;
    if (ads1299_sdatac(device) != ADS1299_OK) rc = 1;
    return rc;
}

/* Replace this with a non-DRDY-context UART/radio/network sender. Returning
 * nonzero means transport is currently busy; the caller retains backpressure
 * by simply trying again later rather than blocking acquisition. */
static int transport_try_send(const ads1299_frame_t *frame)
{
    (void)frame;
    return 0;
}

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t device;
    ads1299_device_id_t identity;
    ads1299_frame_t frame;
    frame_queue_t queue = {0};

    if (cc1312r7_ads1299_make_hal(&hal) != CC1312R7_ADS1299_BIND_OK) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&device, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&device) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&device) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&device, &identity) != ADS1299_OK ||
        !identity.is_ads1299_family) return 6;
    if (ads1299_set_data_rate(&device, ADS1299_DR_250SPS) != ADS1299_OK) return 7;

    for (uint8_t ch = 1u; ch <= identity.channel_count; ++ch) {
        if (ads1299_set_channel(&device, ch, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK)
            return 8;
    }

    if (ads1299_rdatac(&device) != ADS1299_OK) return 9;
    if (ads1299_start(&device) != ADS1299_OK) {
        (void)ads1299_sdatac(&device);
        return 10;
    }

    for (unsigned n = 0; n < STREAM_CAPTURE_FRAMES; ++n) {
        if (ads1299_wait_drdy(&device, 100000u, 100u) != ADS1299_OK) {
            (void)clean_stop(&device);
            return 11;
        }
        if (ads1299_read_frame_continuous(&device, &frame) != ADS1299_OK) {
            (void)clean_stop(&device);
            return 12;
        }

        /* Never wait for radio/UART/network here. Overflow is bounded and
         * observable through queue.dropped instead of corrupting timing. */
        (void)queue_push(&queue, &frame);

        /* Demonstration only: production wireless firmware should perform
         * this pop/send work in a separate task or event loop. */
        if (queue_pop(&queue, &frame) == 0) {
            if (transport_try_send(&frame) != 0) {
                /* A real adapter should retain/requeue this frame according
                 * to its transport policy. This example never blocks DRDY. */
            }
        }
    }

    if (clean_stop(&device) != 0) return 13;

    /* queue.dropped is the explicit stream-overflow diagnostic. Applications
     * should report it after capture or through a low-priority status path. */
    return (queue.dropped == 0u) ? 0 : 14;
}
