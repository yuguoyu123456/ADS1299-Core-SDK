/* MIMXRT1189 beginner step 5: bounded 250-SPS EEG streaming handoff.
 *
 * Acquisition never waits for a slow transport. DRDY polling reads one
 * ADS1299 frame and enqueues it into a fixed-size ring. A board/application
 * transport may drain frames with ads1299_stream_pop(). When the consumer
 * falls behind, the newest frame is dropped and overflow_count records it.
 *
 * ADS1299 register semantics remain in the shared core.
 */
#include "ads1299.h"
#include "ads1299_spi.h"
#include "stream_bounded.h"

extern int board_ads1299_hal(ads1299_platform_hal_t *hal);

#define ADS1299_STREAM_QUEUE_DEPTH 16u

static ads1299_stream_item_t stream_queue[ADS1299_STREAM_QUEUE_DEPTH];
static volatile uint32_t stream_head;
static volatile uint32_t stream_tail;
static volatile uint32_t stream_sequence;

volatile int ads1299_stream_result = -1;
volatile uint32_t ads1299_stream_overflow_count;
volatile uint32_t ads1299_stream_frame_count;

int ads1299_stream_pop(ads1299_stream_item_t *out)
{
    uint32_t tail;

    if (out == 0) {
        return -1;
    }
    tail = stream_tail;
    if (tail == stream_head) {
        return 0;
    }
    *out = stream_queue[tail];
    stream_tail = (tail + 1u) % ADS1299_STREAM_QUEUE_DEPTH;
    return 1;
}

static void stream_push(const ads1299_frame_t *frame)
{
    uint32_t head = stream_head;
    uint32_t next = (head + 1u) % ADS1299_STREAM_QUEUE_DEPTH;

    if (next == stream_tail) {
        ++ads1299_stream_overflow_count;
        return;
    }
    stream_queue[head].frame = *frame;
    stream_queue[head].sequence = stream_sequence++;
    stream_head = next;
    ++ads1299_stream_frame_count;
}

int main(void)
{
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t mcu;
    ads1299_t dev;
    ads1299_frame_t frame;
    ads1299_device_id_t id;
    uint8_t channel;

    if (board_ads1299_hal(&hal) != 0) {
        ads1299_stream_result = 1;
        return 1;
    }
    if (ads1299_mcu_port_init(&mcu, &hal) != 0) {
        ads1299_stream_result = 2;
        return 2;
    }

    ads1299_port_t port = ads1299_mcu_make_port(&mcu);
    if (ads1299_init(&dev, &port) != ADS1299_OK) {
        ads1299_stream_result = 3;
        return 3;
    }
    if (ads1299_hardware_reset(&dev) != ADS1299_OK) {
        ads1299_stream_result = 4;
        return 4;
    }
    if (ads1299_sdatac(&dev) != ADS1299_OK) {
        ads1299_stream_result = 5;
        return 5;
    }
    if (ads1299_read_device_id(&dev, &id) != ADS1299_OK || !id.is_ads1299_family) {
        ads1299_stream_result = 6;
        return 6;
    }
    if (ads1299_set_data_rate(&dev, ADS1299_DR_250SPS) != ADS1299_OK) {
        ads1299_stream_result = 7;
        return 7;
    }
    for (channel = 1u; channel <= id.channel_count; ++channel) {
        if (ads1299_set_channel(&dev, channel, ADS1299_GAIN_24,
                                ADS1299_MUX_NORMAL, 0, 0) != ADS1299_OK) {
            ads1299_stream_result = 8;
            return 8;
        }
    }
    if (ads1299_rdatac(&dev) != ADS1299_OK) {
        ads1299_stream_result = 9;
        return 9;
    }
    if (ads1299_start(&dev) != ADS1299_OK) {
        ads1299_stream_result = 10;
        return 10;
    }

    ads1299_stream_result = 0;
    for (;;) {
        if (port.drdy_read(port.user) == 0) {
            if (ads1299_read_frame_continuous(&dev, &frame) != ADS1299_OK) {
                ads1299_stream_result = 11;
                return 11;
            }
            stream_push(&frame);
        }

        /* UART/USB/Ethernet output belongs in a consumer, never here. */
    }
}
