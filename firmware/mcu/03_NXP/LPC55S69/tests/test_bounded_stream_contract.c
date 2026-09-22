/* Host-side compile/runtime smoke test for the LPC55S69 bounded-stream public
 * consumer contract.  This intentionally does not require ADS1299 hardware or
 * MCUXpresso: it protects the transport-facing type/API from drifting away
 * from examples/bounded_stream.c.
 */
#include <assert.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

/* A transport consumer only needs the public contract.  The real producer is
 * supplied by bounded_stream.c on target; these definitions let this file be
 * compiled and run standalone by a host compiler. */
volatile uint32_t ads1299_stream_dropped;
volatile uint32_t ads1299_stream_produced;

int ads1299_stream_try_pop(ads1299_stream_item_t *out)
{
    if (out == 0) {
        return 0;
    }
    out->sequence = 42u;
    return 1;
}

int main(void)
{
    ads1299_stream_item_t item = {0};

    assert(ads1299_stream_try_pop(0) == 0);
    assert(ads1299_stream_try_pop(&item) == 1);
    assert(item.sequence == 42u);

    ads1299_stream_produced = 7u;
    ads1299_stream_dropped = 2u;
    assert(ads1299_stream_produced == 7u);
    assert(ads1299_stream_dropped == 2u);

    /* Compile-time use of the frame member is the important contract check. */
    (void)item.frame;
    return 0;
}
