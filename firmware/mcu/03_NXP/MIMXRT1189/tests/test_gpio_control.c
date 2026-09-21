#include <assert.h>
#include <stddef.h>

#include "ads1299_gpio.h"

typedef struct {
    ads1299_platform_pin_t pin;
    int level;
    unsigned writes;
} fake_gpio_t;

static void fake_pin_write(void *user, ads1299_platform_pin_t pin, int level)
{
    fake_gpio_t *gpio = (fake_gpio_t *)user;
    gpio->pin = pin;
    gpio->level = level;
    gpio->writes++;
}

static void expect_write(fake_gpio_t *gpio,
                         ads1299_platform_pin_t pin,
                         int level,
                         unsigned writes)
{
    assert(gpio->pin == pin);
    assert(gpio->level == level);
    assert(gpio->writes == writes);
}

void test_control_gpio_routing(void)
{
    fake_gpio_t gpio = {ADS1299_PIN_DRDY, -1, 0};
    ads1299_mcu_port_t port = {0};

    /* Missing context/callback must be harmless during early board bring-up. */
    ads1299_port_cs_write(NULL, 0);
    ads1299_mcu_cs_low(NULL);
    ads1299_mcu_reset(&port, 0);
    assert(gpio.writes == 0);

    port.hal.user = &gpio;
    port.hal.pin_write = fake_pin_write;

    ads1299_port_cs_write(&port, 0);
    expect_write(&gpio, ADS1299_PIN_CS, 0, 1);
    ads1299_port_cs_write(&port, 7); /* public callback normalizes to logic high */
    expect_write(&gpio, ADS1299_PIN_CS, 1, 2);

    ads1299_port_reset_write(&port, 0);
    expect_write(&gpio, ADS1299_PIN_RESET, 0, 3);
    ads1299_port_pwdn_write(&port, 1);
    expect_write(&gpio, ADS1299_PIN_PWDN, 1, 4);
    ads1299_port_start_write(&port, 1);
    expect_write(&gpio, ADS1299_PIN_START, 1, 5);

    ads1299_mcu_cs_low(&port);
    expect_write(&gpio, ADS1299_PIN_CS, 0, 6);
    ads1299_mcu_cs_high(&port);
    expect_write(&gpio, ADS1299_PIN_CS, 1, 7);
    ads1299_mcu_reset(&port, 1);
    expect_write(&gpio, ADS1299_PIN_RESET, 1, 8);
    ads1299_mcu_start(&port, 0);
    expect_write(&gpio, ADS1299_PIN_START, 0, 9);
}
