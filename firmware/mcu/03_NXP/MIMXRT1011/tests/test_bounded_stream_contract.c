/* Host-side compile/smoke test for the public MIMXRT1011 bounded-stream
 * consumer contract.  This intentionally does not require MCUXpresso or
 * hardware: it protects the type/API boundary used by foreground transports.
 */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../examples/bounded_stream.h"

static void contract_compiles(void)
{
    ads1299_stream_item_t item = {0};
    int (*try_pop_fn)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
    volatile uint32_t *produced = &ads1299_stream_produced;
    volatile uint32_t *dropped = &ads1299_stream_dropped;

    (void)try_pop_fn;
    (void)produced;
    (void)dropped;

    /* The transport contract must carry a shared-core decoded frame plus a
     * monotonically assigned platform sequence number; no private ADS1299
     * register representation belongs in this boundary. */
    assert(sizeof(item.frame) == sizeof(ads1299_frame_t));
    assert(sizeof(item.sequence) == sizeof(uint32_t));
    assert(offsetof(ads1299_stream_item_t, frame) == 0u);
}

int main(void)
{
    contract_compiles();
    return 0;
}
