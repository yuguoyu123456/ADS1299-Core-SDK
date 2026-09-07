#ifndef ADS1299_MCUXPRESSO_INIT_H
#define ADS1299_MCUXPRESSO_INIT_H
#include "ads1299_mcuxpresso_hal.h"

/* Caller owns clock roots and pin mux. Initialize only after those are valid.
 * This function configures hardware, never ADS1299 registers or commands.
 * Context and peripheral ownership must remain exclusive during acquisition. */
int ads1299_mcuxpresso_init(ads1299_platform_hal_t *hal,
                          ads1299_mcuxpresso_ctx_t *ctx,
                          uint32_t source_clock_hz, uint32_t spi_clock_hz);
#endif
