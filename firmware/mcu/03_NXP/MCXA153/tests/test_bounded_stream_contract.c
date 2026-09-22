#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

/*
 * Host-side compile/smoke contract for the transport-facing stream API.
 * This deliberately does not require MCUXpresso or ADS1299 hardware: its job
 * is to catch accidental public type/prototype drift while the platform leaf
 * is still TEMPLATE / not board-verified.
 */
int main(void)
{
    ads1299_stream_item_t item = {0};
    int (*try_pop)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
    volatile uint32_t *produced = &ads1299_stream_produced;
    volatile uint32_t *dropped = &ads1299_stream_dropped;

    assert(sizeof(item.sequence) == sizeof(uint32_t));
    assert(try_pop != NULL);
    assert(produced != NULL);
    assert(dropped != NULL);

    return 0;
}
