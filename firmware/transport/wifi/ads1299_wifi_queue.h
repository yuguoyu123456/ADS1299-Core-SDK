#ifndef ADS1299_WIFI_QUEUE_H
#define ADS1299_WIFI_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADS1299_WIFI_QUEUE_DEPTH
#define ADS1299_WIFI_QUEUE_DEPTH 32u
#endif

typedef enum {
    ADS1299_WIFI_DROP_NEWEST = 0,
    ADS1299_WIFI_DROP_OLDEST = 1
} ads1299_wifi_drop_policy_t;

typedef struct {
    uint8_t packets[ADS1299_WIFI_QUEUE_DEPTH][ADS1299_PACKET_SIZE];
    size_t head;
    size_t tail;
    size_t count;
    size_t high_watermark;
    uint32_t accepted_packets;
    uint32_t popped_packets;
    uint32_t dropped_newest;
    uint32_t dropped_oldest;
    uint32_t invalid_packets;
} ads1299_wifi_queue_t;

void ads1299_wifi_queue_init(ads1299_wifi_queue_t *queue);

/* Push one already-encoded canonical packet.
 *  0: accepted without dropping another packet
 *  1: accepted after dropping the oldest queued packet
 * -1: invalid argument
 * -2: canonical packet validation failed
 * -3: queue full and DROP_NEWEST policy rejected this packet
 */
int ads1299_wifi_queue_push(ads1299_wifi_queue_t *queue,
                            const uint8_t packet[ADS1299_PACKET_SIZE],
                            ads1299_wifi_drop_policy_t policy);

/* Copy and remove the oldest queued packet.
 * Returns 1 when a packet was produced, 0 when empty, negative on error.
 */
int ads1299_wifi_queue_pop(ads1299_wifi_queue_t *queue,
                           uint8_t packet_out[ADS1299_PACKET_SIZE]);

/* Inspect the oldest packet without removing it. Pointer remains valid only
 * until the queue is modified.
 */
const uint8_t *ads1299_wifi_queue_peek(const ads1299_wifi_queue_t *queue);

size_t ads1299_wifi_queue_count(const ads1299_wifi_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
