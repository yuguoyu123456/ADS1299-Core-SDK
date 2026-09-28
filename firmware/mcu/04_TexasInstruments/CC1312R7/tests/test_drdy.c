#include <assert.h>
#include <string.h>
#include "ads1299_drdy.h"

typedef struct {
    unsigned reads;
    ads1299_platform_pin_t last_pin;
    int level;
} drdy_fake_t;

static int fake_pin_read(void *user, ads1299_platform_pin_t pin)
{
    drdy_fake_t *fake = (drdy_fake_t *)user;
    ++fake->reads;
    fake->last_pin = pin;
    return fake->level;
}

void test_port_drdy(void)
{
    drdy_fake_t fake;
    ads1299_platform_hal_t hal;
    ads1299_mcu_port_t ctx;

    memset(&fake, 0, sizeof(fake));
    memset(&hal, 0, sizeof(hal));
    hal.user = &fake;
    hal.pin_read = fake_pin_read;
    ctx.hal = hal;

    fake.level = 0;
    assert(ads1299_mcu_data_ready(&ctx) == 1);
    assert(fake.reads == 1);
    assert(fake.last_pin == ADS1299_PIN_DRDY);
    assert(ads1299_port_drdy_read(&ctx) == 0);
    assert(fake.reads == 2);
    assert(fake.last_pin == ADS1299_PIN_DRDY);

    fake.level = 1;
    assert(ads1299_mcu_data_ready(&ctx) == 0);
    assert(ads1299_port_drdy_read(&ctx) == 1);

    assert(ads1299_mcu_data_ready(NULL) == 0);
    assert(ads1299_port_drdy_read(NULL) == 1);

    ctx.hal.pin_read = NULL;
    assert(ads1299_mcu_data_ready(&ctx) == 0);
    assert(ads1299_port_drdy_read(&ctx) == 1);
}
