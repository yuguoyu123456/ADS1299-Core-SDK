#include "ads1299_wifi_queue.h"

#include <string.h>

void ads1299_wifi_queue_init(ads1299_wifi_queue_t *queue)
{
    if (queue) memset(queue, 0, sizeof(*queue));
}

static void drop_oldest(ads1299_wifi_queue_t *queue)
{
    queue->tail = (queue->tail + 1u) % ADS1299_WIFI_QUEUE_DEPTH;
    queue->count--;
    queue->dropped_oldest++;
}

int ads1299_wifi_queue_push(ads1299_wifi_queue_t *queue,
                            const uint8_t packet[ADS1299_PACKET_SIZE],
                            ads1299_wifi_drop_policy_t policy)
{
    int dropped = 0;

    if (!queue || !packet) return -1;
    if (ADS1299_WIFI_QUEUE_DEPTH == 0u) return -1;
    if (ads1299_packet_validate(packet, ADS1299_PACKET_SIZE) != ADS1299_PACKET_OK) {
        queue->invalid_packets++;
        return -2;
    }

    if (queue->count == ADS1299_WIFI_QUEUE_DEPTH) {
        if (policy == ADS1299_WIFI_DROP_NEWEST) {
            queue->dropped_newest++;
            return -3;
        }
        drop_oldest(queue);
        dropped = 1;
    }

    memcpy(queue->packets[queue->head], packet, ADS1299_PACKET_SIZE);
    queue->head = (queue->head + 1u) % ADS1299_WIFI_QUEUE_DEPTH;
    queue->count++;
    queue->accepted_packets++;

    if (queue->count > queue->high_watermark)
        queue->high_watermark = queue->count;

    return dropped;
}

int ads1299_wifi_queue_pop(ads1299_wifi_queue_t *queue,
                           uint8_t packet_out[ADS1299_PACKET_SIZE])
{
    if (!queue || !packet_out) return -1;
    if (queue->count == 0u) return 0;

    memcpy(packet_out, queue->packets[queue->tail], ADS1299_PACKET_SIZE);
    queue->tail = (queue->tail + 1u) % ADS1299_WIFI_QUEUE_DEPTH;
    queue->count--;
    queue->popped_packets++;
    return 1;
}

const uint8_t *ads1299_wifi_queue_peek(const ads1299_wifi_queue_t *queue)
{
    if (!queue || queue->count == 0u) return 0;
    return queue->packets[queue->tail];
}

size_t ads1299_wifi_queue_count(const ads1299_wifi_queue_t *queue)
{
    return queue ? queue->count : 0u;
}
