#include <assert.h>
#include <stddef.h>

#include "ads1299_drdy.h"

typedef struct {
    int level;
    ads1299_platform_pin_t last_pin;
    unsigned reads;
} fake_gpio_t;

static int fake_pin_read(void *user, ads1299_platform_pin_t pin)
{
    fake_gpio_t *gpio = (fake_gpio_t *)user;
    gpio->last_pin = pin;
    gpio->reads++;
    return gpio->level;
}

int main(void)
{
    fake_gpio_t gpio = {1, ADS1299_PIN_CS, 0};
    ads1299_mcu_port_t port = {0};

    /* Missing context/callback must fail safe: never report data ready. */
    assert(ads1299_mcu_data_ready(NULL) == 0);
    assert(ads1299_port_drdy_read(NULL) == 1);
    assert(ads1299_mcu_data_ready(&port) == 0);

    port.hal.user = &gpio;
    port.hal.pin_read = fake_pin_read;

    /* ADS1299 DRDY is active-low: high means no sample is ready. */
    gpio.level = 1;
    assert(ads1299_mcu_data_ready(&port) == 0);
    assert(ads1299_port_drdy_read(&port) == 1);

    /* Low DRDY means a conversion frame is ready. */
    gpio.level = 0;
    assert(ads1299_mcu_data_ready(&port) == 1);
    assert(ads1299_port_drdy_read(&port) == 0);

    assert(gpio.last_pin == ADS1299_PIN_DRDY);
    assert(gpio.reads == 4);
    return 0;
}
