#ifndef ADS1299_C6_HAL_H
#define ADS1299_C6_HAL_H
#include "ads1299_spi.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
/* Static lifetime, single task, dedicated bus, DMA disabled, queue_size=1.
 * After timeout, do not free/reset this context or remove the device until the
 * outstanding descriptor is returned. Recovery is application-owned. */
typedef struct {
    spi_device_handle_t spi;
    gpio_num_t pins[5];
    TickType_t timeout_ticks;
    spi_transaction_t transaction;
    uint8_t tx[32],rx[32];
    int fault,in_flight;
} ads1299_c6_context_t;
int ads1299_c6_make_hal(ads1299_c6_context_t *ctx,ads1299_platform_hal_t *hal);
#endif
