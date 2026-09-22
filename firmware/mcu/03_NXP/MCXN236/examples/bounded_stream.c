/* MCXN236 bounded acquisition example.
 * Producer: ADS1299 DRDY polling + frame read. Consumer/transport is deliberately
 * separate so UART/USB/network work never needs to run in the acquisition path. */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "bounded_stream.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

#define ADS1299_STREAM_QUEUE_DEPTH 16u

static ads1299_stream_item_t queue[ADS1299_STREAM_QUEUE_DEPTH];
static volatile uint32_t queue_head;
static volatile uint32_t queue_tail;
volatile uint32_t ads1299_stream_dropped;
volatile uint32_t ads1299_stream_produced;

int ads1299_stream_try_pop(ads1299_stream_item_t *out) {
    uint32_t tail;
    if (out == 0) return 0;
    tail = queue_tail;
    if (tail == queue_head) return 0;
    *out = queue[tail];
    queue_tail = (tail + 1u) % ADS1299_STREAM_QUEUE_DEPTH;
    return 1;
}

static void queue_push_drop_newest(const ads1299_frame_t *frame) {
    uint32_t head = queue_head;
    uint32_t next = (head + 1u) % ADS1299_STREAM_QUEUE_DEPTH;
    if (next == queue_tail) {
        ++ads1299_stream_dropped;
        return;
    }
    queue[head].frame = *frame;
    queue[head].sequence = ads1299_stream_produced++;
    queue_head = next;
}

int main(void) {
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    uint8_t channel;

    if (board_ads1299_hal(&hal) != 0) return 1;
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) return 2;
    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
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
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) return 11;
            queue_push_drop_newest(&frame);
        }
        /* Drain ads1299_stream_try_pop() from foreground/transport code.
         * If producer and consumer move to different ISR/RTOS contexts, add
         * the synchronization primitive appropriate to that application. */
    }
}
