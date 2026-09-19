/* MIMXRT1052 beginner step 5: bounded 250-SPS ADS1299 streaming.
 *
 * Acquisition never waits for the consumer. A fixed-size SPSC ring buffers
 * complete shared-core frames; if the consumer falls behind, the newest frame
 * is dropped and ads1299_stream_dropped is incremented. Replace
 * board_ads1299_stream_write() with UART/USB/network transport in the board
 * layer; do not put a blocking transport in the DRDY acquisition path.
 */
#include "ads1299.h"
#include "ads1299_spi.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);
extern int board_ads1299_stream_write(const ads1299_frame_t *frame);

#define ADS1299_STREAM_QUEUE_DEPTH 8u

typedef struct {
    ads1299_frame_t frame[ADS1299_STREAM_QUEUE_DEPTH];
    volatile uint32_t head;
    volatile uint32_t tail;
} ads1299_stream_queue_t;

static ads1299_stream_queue_t g_queue;
volatile uint32_t ads1299_stream_acquired;
volatile uint32_t ads1299_stream_sent;
volatile uint32_t ads1299_stream_dropped;
volatile uint32_t ads1299_stream_transport_errors;

static int queue_push(const ads1299_frame_t *frame) {
    uint32_t head = g_queue.head;
    uint32_t next = (head + 1u) % ADS1299_STREAM_QUEUE_DEPTH;
    if (next == g_queue.tail) return -1;
    g_queue.frame[head] = *frame;
    g_queue.head = next;
    return 0;
}

static int queue_pop(ads1299_frame_t *frame) {
    uint32_t tail = g_queue.tail;
    if (tail == g_queue.head) return 0;
    *frame = g_queue.frame[tail];
    g_queue.tail = (tail + 1u) % ADS1299_STREAM_QUEUE_DEPTH;
    return 1;
}

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    ads1299_port_t port;
    uint8_t channel;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) return 3;
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) return 4;
    if (ads1299_sdatac(&dev) != ADS1299_OK) return 5;
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) return 6;
    if (ads1299_set_data_rate(&dev, ADS1299_DR_250SPS) != ADS1299_OK) return 7;
    for (channel = 1u; channel <= id.channel_count; ++channel) {
        if (ads1299_set_channel(&dev, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) return 8;
    }
    if (ads1299_rdatac(&dev) != ADS1299_OK) return 9;
    if (ads1299_start(&dev) != ADS1299_OK) return 10;

    for (;;) {
        int ready = port.drdy_read(port.user);
        if (ready < 0) return 11;
        if (ready == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) return 12;
            ++ads1299_stream_acquired;
            if (queue_push(&frame) != 0) ++ads1299_stream_dropped;
        }

        /* One bounded consumer attempt per loop. For DMA/RTOS integrations,
         * move this consumer to a lower-priority task while retaining the
         * fixed queue and overflow accounting contract. */
        if (queue_pop(&frame) != 0) {
            if (board_ads1299_stream_write(&frame) == 0) {
                ++ads1299_stream_sent;
            } else {
                ++ads1299_stream_transport_errors;
            }
        }
    }
}
