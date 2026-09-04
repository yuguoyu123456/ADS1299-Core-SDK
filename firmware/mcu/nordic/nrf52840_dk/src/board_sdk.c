#include "board_sdk.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

static const struct device *const gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));

/* Use Arduino D2..D10 on nRF52840 DK. Zephyr's board DTS maps these to
 * P1.03, P1.04, P1.05, P1.06, P1.07, P1.08, P1.10, P1.11 and P1.12. */
static const gpio_pin_t board_pins[] = {
    [BOARD_PIN_SCK] = 3u,
    [BOARD_PIN_MISO] = 4u,
    [BOARD_PIN_MOSI] = 5u,
    [BOARD_PIN_CS] = 6u,
    [BOARD_PIN_DRDY] = 7u,
    [BOARD_PIN_RESET] = 8u,
    [BOARD_PIN_PWDN] = 10u,
    [BOARD_PIN_START] = 11u,
    [BOARD_PIN_TX] = 12u,
};

static gpio_pin_t pin_number(board_sdk_pin_t pin)
{
    return board_pins[(unsigned int)pin];
}

static int configure_output(board_sdk_pin_t pin)
{
    return gpio_pin_configure(gpio1, pin_number(pin), GPIO_OUTPUT_INACTIVE);
}

static int configure_input(board_sdk_pin_t pin)
{
    return gpio_pin_configure(gpio1, pin_number(pin), GPIO_INPUT);
}

int board_sdk_init(void)
{
    if (!device_is_ready(gpio1)) {
        return -1;
    }

    if (configure_output(BOARD_PIN_SCK) != 0 ||
        configure_input(BOARD_PIN_MISO) != 0 ||
        configure_output(BOARD_PIN_MOSI) != 0 ||
        configure_output(BOARD_PIN_CS) != 0 ||
        configure_input(BOARD_PIN_DRDY) != 0 ||
        configure_output(BOARD_PIN_RESET) != 0 ||
        configure_output(BOARD_PIN_PWDN) != 0 ||
        configure_output(BOARD_PIN_START) != 0 ||
        configure_output(BOARD_PIN_TX) != 0) {
        return -1;
    }
    return 0;
}

void board_sdk_write_pin(board_sdk_pin_t pin, int level)
{
    (void)gpio_pin_set(gpio1, pin_number(pin), level != 0 ? 1 : 0);
}

int board_sdk_read_pin(board_sdk_pin_t pin)
{
    const int value = gpio_pin_get(gpio1, pin_number(pin));
    return value > 0 ? 1 : 0;
}

void board_sdk_delay_us(uint32_t us)
{
    k_busy_wait(us);
}

void board_sdk_fatal(const char *reason)
{
    (void)reason;
    for (;;) {
        (void)gpio_pin_set(gpio1, pin_number(BOARD_PIN_TX), 0);
        k_busy_wait(1000u);
    }
}
