#ifndef ADS1299_MCU_SPI_H
#define ADS1299_MCU_SPI_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_port.h"

typedef enum {
    ADS1299_PIN_CS,
    ADS1299_PIN_RESET,
    ADS1299_PIN_PWDN,
    ADS1299_PIN_START,
    ADS1299_PIN_DRDY
} ads1299_platform_pin_t;

typedef struct {
    void *user;
    int (*spi_transfer)(void *user, const uint8_t *tx, uint8_t *rx, size_t len);
    void (*pin_write)(void *user, ads1299_platform_pin_t pin, int level);
    int (*pin_read)(void *user, ads1299_platform_pin_t pin);
    void (*delay_us)(void *user, uint32_t us);
} ads1299_platform_hal_t;

typedef struct {
    ads1299_platform_hal_t hal;
} ads1299_mcu_port_t;

int ads1299_mcu_port_init(ads1299_mcu_port_t *ctx,
                          const ads1299_platform_hal_t *hal);
ads1299_port_t ads1299_mcu_make_port(ads1299_mcu_port_t *ctx);
int ads1299_mcu_spi_write(ads1299_mcu_port_t *ctx,
                          const uint8_t *data, size_t len);
int ads1299_mcu_spi_read(ads1299_mcu_port_t *ctx,
                         uint8_t *data, size_t len);
int ads1299_mcu_spi_transfer(ads1299_mcu_port_t *ctx,
                             const uint8_t *tx, uint8_t *rx, size_t len);

#endif
