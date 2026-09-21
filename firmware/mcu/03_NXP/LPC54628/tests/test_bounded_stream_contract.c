/* Compile-time smoke test for the LPC54628 bounded-stream consumer contract.
 * This deliberately does not claim target or hardware validation. */
#include <stdint.h>
#include "../examples/bounded_stream.h"

static int consume_one(void)
{
    ads1299_stream_item_t item;
    int available = ads1299_stream_try_pop(&item);

    if (available != 0) {
        volatile uint32_t sequence = item.sequence;
        volatile int32_t channel0 = item.frame.channel[0];
        (void)sequence;
        (void)channel0;
    }

    return available;
}

int ads1299_lpc54628_test_bounded_stream_contract(void)
{
    volatile uint32_t produced = ads1299_stream_produced;
    volatile uint32_t dropped = ads1299_stream_dropped;
    (void)produced;
    (void)dropped;

    /* NULL is part of the documented non-blocking consumer contract. */
    if (ads1299_stream_try_pop(0) != 0) {
        return 1;
    }

    (void)consume_one;
    return 0;
}
