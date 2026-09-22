#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

/* Host-side compile/smoke test for the transport-facing bounded-stream API.
 * It intentionally does not require MCUXpresso or ADS1299 hardware. */
int main(void)
{
    ads1299_stream_item_t item = {0};
    int (*try_pop)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
    volatile uint32_t *produced = &ads1299_stream_produced;
    volatile uint32_t *dropped = &ads1299_stream_dropped;

    assert(try_pop != NULL);
    assert(produced != NULL);
    assert(dropped != NULL);
    assert(sizeof(item.sequence) == sizeof(uint32_t));
    assert(sizeof(item.frame) == sizeof(ads1299_frame_t));

    return 0;
}
