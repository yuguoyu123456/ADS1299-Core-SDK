#ifndef ADS1299_PORT_H
#define ADS1299_PORT_H

#include <stddef.h>
#include <stdint.h>

typedef int (*ads1299_spi_transfer_fn)(void *user,
                                       const uint8_t *tx,
                                       uint8_t *rx,
                                       size_t len);
typedef void (*ads1299_gpio_write_fn)(void *user, int level);
typedef int (*ads1299_gpio_read_fn)(void *user);
typedef void (*ads1299_delay_us_fn)(void *user, uint32_t us);

typedef struct {
    void *user;
    ads1299_spi_transfer_fn spi_transfer;
    ads1299_gpio_write_fn cs_write;
    ads1299_gpio_write_fn reset_write;
    ads1299_gpio_write_fn pwdn_write;
    ads1299_gpio_write_fn start_write;
    ads1299_gpio_read_fn drdy_read;
    ads1299_delay_us_fn delay_us;
} ads1299_port_t;

#endif
