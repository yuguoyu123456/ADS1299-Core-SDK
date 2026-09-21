#include <assert.h>
#include <stdint.h>

#include "ads1299_gpio.h"
#include "ads1299_spi.h"

static uint32_t delay_call_count;
static uint32_t delay_total_us;
static uint32_t delay_last_us;

static void fake_delay_us(void *user, uint32_t us)
{
    (void)user;
    delay_call_count++;
    delay_total_us += us;
    delay_last_us = us;
}

void test_delay_routing(void)
{
    ads1299_mcu_port_t ctx = {0};

    ctx.hal.delay_us = fake_delay_us;

    delay_call_count = 0u;
    delay_total_us = 0u;
    delay_last_us = 0u;
    ads1299_mcu_delay_us(&ctx, 37u);
    assert(delay_call_count == 1u);
    assert(delay_total_us == 37u);
    assert(delay_last_us == 37u);

    delay_call_count = 0u;
    delay_total_us = 0u;
    delay_last_us = 0u;
    ads1299_mcu_delay_ms(&ctx, 3u);
    assert(delay_call_count == 3u);
    assert(delay_total_us == 3000u);
    assert(delay_last_us == 1000u);

    delay_call_count = 0u;
    delay_total_us = 0u;
    ads1299_mcu_delay_ms(&ctx, 0u);
    assert(delay_call_count == 0u);
    assert(delay_total_us == 0u);

    /* Missing callbacks and NULL contexts are intentionally safe no-ops. */
    ctx.hal.delay_us = 0;
    ads1299_mcu_delay_us(&ctx, 10u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    ads1299_mcu_delay_us(0, 10u);
    ads1299_mcu_delay_ms(0, 2u);
}
