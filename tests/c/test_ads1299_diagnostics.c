#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ads1299_diagnostics.h"

static int near(double a, double b, double tol) {
    const double d = (a >= b) ? (a - b) : (b - a);
    return d <= tol;
}

static void test_temperature_equation3(void) {
    double c = 0.0;
    assert(ads1299_temperature_c_from_microvolts(145300.0, &c) == ADS1299_OK);
    assert(near(c, 25.0, 1e-12));
    assert(ads1299_temperature_c_from_microvolts(145790.0, &c) == ADS1299_OK);
    assert(near(c, 26.0, 1e-12));
    assert(ads1299_temperature_c_from_microvolts(144810.0, &c) == ADS1299_OK);
    assert(near(c, 24.0, 1e-12));
    assert(ads1299_temperature_c_from_microvolts(145300.0, NULL) == ADS1299_EINVAL);
}

static void test_mvdd_mux_equations(void) {
    double v = 0.0;
    assert(ads1299_mvdd_mux_input_volts(1u, 5.0, 0.0, 3.3, &v) == ADS1299_OK);
    assert(near(v, 2.5, 1e-12));
    assert(ads1299_mvdd_mux_input_volts(8u, 5.0, 0.0, 3.3, &v) == ADS1299_OK);
    assert(near(v, 2.5, 1e-12));
    assert(ads1299_mvdd_mux_input_volts(3u, 5.0, 0.0, 3.3, &v) == ADS1299_OK);
    assert(near(v, 0.825, 1e-12));
    assert(ads1299_mvdd_mux_input_volts(4u, 2.5, -2.5, 1.8, &v) == ADS1299_OK);
    assert(near(v, 0.45, 1e-12));
    assert(ads1299_mvdd_mux_input_volts(1u, 2.5, -2.5, 3.3, &v) == ADS1299_OK);
    assert(near(v, 0.0, 1e-12));
    assert(ads1299_mvdd_mux_input_volts(0u, 5.0, 0.0, 3.3, &v) == ADS1299_EINVAL);
    assert(ads1299_mvdd_mux_input_volts(9u, 5.0, 0.0, 3.3, &v) == ADS1299_EINVAL);
    assert(ads1299_mvdd_mux_input_volts(3u, 5.0, 0.0, 0.0, &v) == ADS1299_EINVAL);
}

typedef struct {
    uint8_t first_tx[16];
    size_t calls;
} mock_t;

static int spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    mock_t *m = (mock_t *)user;
    if (m->calls < 16u) m->first_tx[m->calls] = (tx && len) ? tx[0] : 0u;
    if (rx) memset(rx, 0, len);
    m->calls++;
    return 0;
}
static void cs_write(void *user, int level) { (void)user; (void)level; }
static void delay_us(void *user, uint32_t us) { (void)user; (void)us; }

static ads1299_t make_dev(mock_t *mock) {
    const ads1299_port_t port = {
        .user = mock,
        .spi_transfer = spi_transfer,
        .cs_write = cs_write,
        .delay_us = delay_us,
    };
    ads1299_t dev;
    assert(ads1299_init(&dev, &port) == ADS1299_OK);
    return dev;
}

static void test_named_diagnostic_profiles(void) {
    mock_t mock = {0};
    ads1299_t dev = make_dev(&mock);
    dev.channel_count = 4u;

    size_t before = mock.calls;
    assert(ads1299_configure_temperature_measurement(
               &dev, 2u, ADS1299_GAIN_12) == ADS1299_OK);
    assert(mock.calls == before + 4u);
    assert(mock.first_tx[before] == ADS1299_CMD_SDATAC);
    assert(mock.first_tx[before + 1u] ==
           (uint8_t)(ADS1299_CMD_WREG | ADS1299_REG_CH2SET));
    assert(mock.first_tx[before + 3u] ==
           (uint8_t)(ADS1299_GAIN_12 | ADS1299_MUX_TEMP));

    before = mock.calls;
    assert(ads1299_configure_supply_measurement(&dev, 3u) == ADS1299_OK);
    assert(mock.first_tx[before] == ADS1299_CMD_SDATAC);
    assert(mock.first_tx[before + 1u] ==
           (uint8_t)(ADS1299_CMD_WREG | ADS1299_REG_CH3SET));
    assert(mock.first_tx[before + 3u] ==
           (uint8_t)(ADS1299_GAIN_1 | ADS1299_MUX_MVDD));

    before = mock.calls;
    assert(ads1299_configure_supply_measurement(&dev, 5u) == ADS1299_EINVAL);
    assert(mock.calls == before);
    assert(ads1299_configure_temperature_measurement(
               &dev, 1u, ADS1299_GAIN_DO_NOT_USE) == ADS1299_EINVAL);
    assert(mock.calls == before);
}

int main(void) {
    test_temperature_equation3();
    test_mvdd_mux_equations();
    test_named_diagnostic_profiles();
    puts("ADS1299 temperature/MVDD diagnostic tests passed");
    return 0;
}
