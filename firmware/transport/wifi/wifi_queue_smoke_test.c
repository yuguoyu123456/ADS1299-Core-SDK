#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_packet.h"
#include "ads1299_wifi_queue.h"

static void make_packet(uint8_t out[ADS1299_PACKET_SIZE], uint32_t sequence)
{
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS];
    size_t i;

    for (i = 0u; i < ADS1299_PACKET_CHANNELS; ++i)
        channels[i] = (int32_t)(sequence * 10u + (uint32_t)i);

    assert(ads1299_packet_encode(out,
                                 ADS1299_PACKET_SIZE,
                                 ADS1299_PACKET_FLAG_DATA_VALID,
                                 sequence,
                                 sequence * 4000u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);
}

int main(void)
{
    ads1299_wifi_queue_t queue;
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint8_t out[ADS1299_PACKET_SIZE];
    ads1299_packet_t decoded;
    size_t i;

    ads1299_wifi_queue_init(&queue);
    assert(ads1299_wifi_queue_count(&queue) == 0u);

    make_packet(packet, 1u);
    assert(ads1299_wifi_queue_push(&queue, packet, ADS1299_WIFI_DROP_NEWEST) == 0);
    assert(ads1299_wifi_queue_count(&queue) == 1u);
    assert(ads1299_wifi_queue_peek(&queue) != 0);
    assert(ads1299_wifi_queue_pop(&queue, out) == 1);
    assert(memcmp(packet, out, ADS1299_PACKET_SIZE) == 0);
    assert(ads1299_wifi_queue_count(&queue) == 0u);

    /* Fill the queue, then verify DROP_NEWEST preserves queued data. */
    ads1299_wifi_queue_init(&queue);
    for (i = 0u; i < ADS1299_WIFI_QUEUE_DEPTH; ++i) {
        make_packet(packet, (uint32_t)i);
        assert(ads1299_wifi_queue_push(&queue, packet, ADS1299_WIFI_DROP_NEWEST) == 0);
    }
    assert(queue.high_watermark == ADS1299_WIFI_QUEUE_DEPTH);
    make_packet(packet, 1000u);
    assert(ads1299_wifi_queue_push(&queue, packet, ADS1299_WIFI_DROP_NEWEST) == -3);
    assert(queue.dropped_newest == 1u);
    assert(ads1299_wifi_queue_count(&queue) == ADS1299_WIFI_QUEUE_DEPTH);

    /* DROP_OLDEST keeps the newest real-time sample and accounts for the loss. */
    assert(ads1299_wifi_queue_push(&queue, packet, ADS1299_WIFI_DROP_OLDEST) == 1);
    assert(queue.dropped_oldest == 1u);
    assert(ads1299_wifi_queue_count(&queue) == ADS1299_WIFI_QUEUE_DEPTH);

    /* Drain and confirm the newest packet eventually appears. */
    while (ads1299_wifi_queue_pop(&queue, out) == 1) {
        assert(ads1299_packet_decode(out, ADS1299_PACKET_SIZE, &decoded) == ADS1299_PACKET_OK);
    }
    assert(decoded.sequence == 1000u);

    /* Invalid CRC must never enter the queue. */
    make_packet(packet, 7u);
    packet[20] ^= 0x01u;
    assert(ads1299_wifi_queue_push(&queue, packet, ADS1299_WIFI_DROP_NEWEST) == -2);
    assert(queue.invalid_packets == 1u);

    return 0;
}
