#ifndef ADS1299_FSP_HAL_H
#define ADS1299_FSP_HAL_H

#include "ads1299_spi.h"
#include "r_spi_api.h"
#include "r_ioport_api.h"

/* Dedicated, interrupt-driven r_spi instance. Storage must have static lifetime.
 * Only one thread may access it; never call from ISR or with IRQs masked.
 * IOPORT must already be open with GPIO direction and safe levels configured.
 * The caller supplies a free-running microsecond clock, wrapping at 2^32.
 * It MUST keep advancing while this adapter waits (including IRQ failure).
 */
typedef struct {
    spi_instance_t const *spi;
    ioport_instance_t const *io;
    bsp_io_port_pin_t pins[5]; /* CS, RESET, PWDN, START, DRDY */
    void *time_user;
    uint32_t (*now_us)(void *user);
    void (*delay_us)(void *user, uint32_t us);
    uint32_t timeout_us; /* 1 .. INT32_MAX, per <= 32-byte chunk */
    spi_cfg_t config; /* persistent copy; original generated config untouched */
    volatile uint32_t event;
    uint8_t tx[32];
    uint8_t rx[32];
    int fault;
    int opened;
} ads1299_fsp_context_t;

/* Use a zero-initialized context; fill only spi/io/pins/time/timeout fields.
 * Owns SPI open/close, not IOPORT. Failure leaves output HAL untouched.
 * No DMA/DTC in this reference; timeout cleanup relies on r_spi Close.
 */
int ads1299_fsp_open(ads1299_fsp_context_t *ctx, ads1299_platform_hal_t *hal);
int ads1299_fsp_close(ads1299_fsp_context_t *ctx);
void ads1299_fsp_callback(spi_callback_args_t *args);

#endif
