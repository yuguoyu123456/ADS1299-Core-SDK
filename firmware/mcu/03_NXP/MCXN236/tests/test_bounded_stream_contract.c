#include <stddef.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

/*
 * Host-side compile/smoke contract for the transport-facing bounded-stream API.
 * This intentionally does not require MCUXpresso or ADS1299 hardware: it makes
 * accidental type/prototype drift visible while the board integration remains
 * TEMPLATE / not build-verified / not board-verified.
 */
static int (*const stream_try_pop_fn)(ads1299_stream_item_t *) =
    ads1299_stream_try_pop;

static volatile uint32_t *const produced_counter = &ads1299_stream_produced;
static volatile uint32_t *const dropped_counter = &ads1299_stream_dropped;

int main(void)
{
    ads1299_stream_item_t item;

    /* The public item must carry one decoded shared-core frame plus a sequence
     * number; transport code must not need a private copy of this type. */
    if (sizeof(item.frame) != sizeof(ads1299_frame_t)) {
        return 1;
    }
    if (sizeof(item.sequence) != sizeof(uint32_t)) {
        return 2;
    }
    if (offsetof(ads1299_stream_item_t, sequence) < sizeof(ads1299_frame_t)) {
        return 3;
    }

    /* Referencing the public symbols makes signature/name drift a compile or
     * link failure when this smoke test is built with bounded_stream.c. */
    if (stream_try_pop_fn == NULL || produced_counter == NULL ||
        dropped_counter == NULL) {
        return 4;
    }

    return 0;
}
