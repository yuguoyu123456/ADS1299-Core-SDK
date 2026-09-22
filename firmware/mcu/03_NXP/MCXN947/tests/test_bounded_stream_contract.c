#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

/*
 * Host-side compile/smoke contract for the MCXN947 bounded-stream interface.
 * This intentionally does not require MCUXpresso or ADS1299 hardware: it
 * protects the transport-facing type and symbol contract while platform
 * integration remains TEMPLATE / not board-verified.
 */

static void test_item_contract(void)
{
    ads1299_stream_item_t item = {0};

    /* The transport item must continue to carry the shared-core decoded frame
     * plus an explicit monotonically increasing 32-bit sequence field. */
    assert(sizeof(item.sequence) == sizeof(uint32_t));
    assert(sizeof(item.frame) == sizeof(ads1299_frame_t));
    assert(offsetof(ads1299_stream_item_t, frame) == 0u);
    assert(offsetof(ads1299_stream_item_t, sequence) >= sizeof(ads1299_frame_t));
}

static void test_public_symbol_types(void)
{
    int (*try_pop_fn)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
    volatile uint32_t *produced = &ads1299_stream_produced;
    volatile uint32_t *dropped = &ads1299_stream_dropped;

    assert(try_pop_fn != NULL);
    assert(produced != NULL);
    assert(dropped != NULL);
}

int main(void)
{
    test_item_contract();
    test_public_symbol_types();
    return 0;
}
