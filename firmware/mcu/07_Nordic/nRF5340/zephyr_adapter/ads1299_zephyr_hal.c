/* Hardware-only adapter; ADS1299 protocol resides exclusively in Core. */
#include "ads1299_spi.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <errno.h>

static const struct device *const bus = DEVICE_DT_GET(DT_NODELABEL(spi4));
static const struct gpio_dt_spec pins[] = {
    [ADS1299_PIN_CS] = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), cs_gpios),
    [ADS1299_PIN_RESET] = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), reset_gpios),
    [ADS1299_PIN_PWDN] = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), pwdn_gpios),
    [ADS1299_PIN_START] = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), start_gpios),
    [ADS1299_PIN_DRDY] = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), drdy_gpios),
};
static const struct spi_config config = {
    .frequency = 1000000u,
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA,
    /* No SPI CS control: the Core's separate GPIO callbacks own CS. */
};
static int io_error;

static int transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t length) {
    (void)user;
    if (io_error) return io_error;
    if (!length || (!tx && !rx)) return -EINVAL;
    for (size_t offset = 0; offset < length;) {
        /* RAM buffers also support EasyDMA when Core TX data lives in Flash. */
        uint8_t output[32] = {0}, input[32];
        size_t count = length - offset;
        if (count > sizeof(output)) count = sizeof(output);
        for (size_t i = 0; tx && i < count; ++i) output[i] = tx[offset + i];
        struct spi_buf out = {.buf = output, .len = count};
        struct spi_buf in = {.buf = input, .len = count};
        const struct spi_buf_set outs = {.buffers = &out, .count = 1};
        const struct spi_buf_set ins = {.buffers = &in, .count = 1};
        int result = spi_transceive(bus, &config, &outs, &ins);
        if (result) return result;
        for (size_t i = 0; rx && i < count; ++i) rx[offset + i] = input[i];
        offset += count;
    }
    return 0;
}

static void write_pin(void *user, ads1299_platform_pin_t pin, int level) {
    (void)user;
    if ((unsigned)pin >= ARRAY_SIZE(pins) || pin == ADS1299_PIN_DRDY) {
        io_error = -EINVAL;
        return;
    }
    int result = gpio_pin_set_raw(pins[pin].port, pins[pin].pin, level != 0);
    if (result) io_error = result;
}

static int read_pin(void *user, ads1299_platform_pin_t pin) {
    (void)user;
    if ((unsigned)pin >= ARRAY_SIZE(pins)) return -EINVAL;
    int result = gpio_pin_get_raw(pins[pin].port, pins[pin].pin);
    if (result < 0) io_error = result;
    return result;
}

static void delay_us(void *user, uint32_t us) {
    (void)user;
    k_busy_wait(us);
}

int board_ads1299_hal(ads1299_platform_hal_t *hal) {
    if (!hal || !device_is_ready(bus)) return -ENODEV;
    for (size_t i = 0; i < ARRAY_SIZE(pins); ++i) {
        if (!gpio_is_ready_dt(&pins[i])) return -ENODEV;
        for (size_t j = 0; j < i; ++j)
            if (pins[i].port == pins[j].port && pins[i].pin == pins[j].pin)
                return -EINVAL;
    }
    for (size_t i = 0; i < ARRAY_SIZE(pins); ++i) {
        gpio_flags_t flags = i == ADS1299_PIN_DRDY ? GPIO_INPUT :
            (i == ADS1299_PIN_START ? GPIO_OUTPUT_LOW : GPIO_OUTPUT_HIGH);
        int result = gpio_pin_configure_dt(&pins[i], flags);
        if (result) return result;
    }
    io_error = 0;
    *hal = (ads1299_platform_hal_t){NULL, transfer, write_pin, read_pin, delay_us};
    return 0;
}
