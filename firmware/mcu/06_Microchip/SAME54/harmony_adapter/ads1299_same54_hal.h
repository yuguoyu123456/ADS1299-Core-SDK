#ifndef ADS1299_SAME54_HAL_H
#define ADS1299_SAME54_HAL_H
#include "ads1299_spi.h"
#include "device.h"

/* Original polling adapter using official DFP peripheral definitions.
 * Board owns clock/mux/direction setup. No Harmony SPI PLIB/ISR/DMA may share
 * this SERCOM. Pins encode group*32+bit, must be bonded and board-verified.
 * delay_us must implement minimum real-time delays at the configured CPU clock.
 */
typedef struct {
    sercom_registers_t *spi;
    uint32_t pins[5]; /* CS RESET PWDN START DRDY */
    uint32_t source_hz;
    uint32_t spi_hz;
    uint32_t poll_limit; /* iteration budget, NOT calibrated microseconds */
    void *delay_user;
    void (*delay_us)(void *, uint32_t);
    int fault;
} ads1299_same54_context_t;

/* PAD0 output / PAD1 clock / PAD3 input. Call only when bus is quiescent.
 * Reinitialization after failure requires board-owned SERCOM reset first.
 */
int ads1299_same54_make_hal(ads1299_same54_context_t *ctx,
                          ads1299_platform_hal_t *hal);
#endif
