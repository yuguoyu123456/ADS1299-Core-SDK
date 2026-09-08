#include "ads1299_frame_ring.h"

#include <string.h>

void ads1299_frame_ring_init(ads1299_frame_ring_t *ring)
{
    if (!ring) return;
    memset(ring, 0, sizeof(*ring));
}

int ads1299_frame_ring_push(ads1299_frame_ring_t *ring,
                            const ads1299_frame_t *frame,
                            uint32_t sequence,
                            uint32_t timestamp_ticks)
{
    ads1299_buffered_frame_t *slot;

    if (!ring || !frame) return -1;
    if (ring->count >= ADS1299_FRAME_RING_CAPACITY) {
        ring->overflows++;
        return -2;
    }

    slot = &ring->slots[ring->write_index];
    slot->sequence = sequence;
    slot->timestamp_ticks = timestamp_ticks;
    slot->frame = *frame;

    ring->write_index = (ring->write_index + 1u) % ADS1299_FRAME_RING_CAPACITY;
    ring->count++;
    ring->pushed++;
    return 0;
}

int ads1299_frame_ring_pop(ads1299_frame_ring_t *ring,
                           ads1299_buffered_frame_t *out)
{
    if (!ring || !out) return -1;
    if (ring->count == 0u) return 1;

    *out = ring->slots[ring->read_index];
    ring->read_index = (ring->read_index + 1u) % ADS1299_FRAME_RING_CAPACITY;
    ring->count--;
    ring->popped++;
    return 0;
}

size_t ads1299_frame_ring_count(const ads1299_frame_ring_t *ring)
{
    return ring ? ring->count : 0u;
}

uint32_t ads1299_frame_ring_overflows(const ads1299_frame_ring_t *ring)
{
    return ring ? ring->overflows : 0u;
}
