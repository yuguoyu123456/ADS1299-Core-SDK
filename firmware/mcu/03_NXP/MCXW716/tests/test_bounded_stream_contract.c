/* Host/compiler smoke test for the MCXW716 bounded-stream public contract.
 *
 * This deliberately does not exercise hardware or the producer loop.  It
 * protects the transport-facing API so BLE/802.15.4/UART/USB consumers can be
 * compiled independently of the DRDY acquisition path.
 */
#include <stdint.h>

#include "../examples/bounded_stream.h"

static void consume_contract(ads1299_stream_item_t *item)
{
    /* Keep both public fields compile-visible without assuming frame internals. */
    ads1299_frame_t *frame = &item->frame;
    uint32_t sequence = item->sequence;
    (void)frame;
    (void)sequence;
}

int main(void)
{
    ads1299_stream_item_t item;
    int (*try_pop)(ads1299_stream_item_t *) = ads1299_stream_try_pop;
    volatile uint32_t *produced = &ads1299_stream_produced;
    volatile uint32_t *dropped = &ads1299_stream_dropped;

    consume_contract(&item);
    (void)try_pop;
    (void)produced;
    (void)dropped;

    /* The contract requires a full shared-core frame plus a 32-bit sequence. */
    return (sizeof(item.frame) == sizeof(ads1299_frame_t) &&
            sizeof(item.sequence) == sizeof(uint32_t)) ? 0 : 1;
}
