#include "ads1299_port.h"

/* Copy this file into a platform port and replace the stubs with the vendor SDK. */

static int platform_spi_transfer(void *user, const uint8_t *tx, uint8_t *rx, size_t len) {
    (void)user; (void)tx; (void)rx; (void)len;
    return -1;
}

static void platform_cs_write(void *user, int level) {
    (void)user; (void)level;
}

static void platform_reset_write(void *user, int level) {
    (void)user; (void)level;
}

static void platform_start_write(void *user, int level) {
    (void)user; (void)level;
}

static int platform_drdy_read(void *user) {
    (void)user;
    return 1;
}

static void platform_delay_us(void *user, uint32_t us) {
    (void)user; (void)us;
}

ads1299_port_t ads1299_make_platform_port(void *user) {
    ads1299_port_t p = {
        .user = user,
        .spi_transfer = platform_spi_transfer,
        .cs_write = platform_cs_write,
        .reset_write = platform_reset_write,
        .start_write = platform_start_write,
        .drdy_read = platform_drdy_read,
        .delay_us = platform_delay_us,
    };
    return p;
}
