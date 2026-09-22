#ifndef LPC55S69_BOARD_ADS1299_BINDING_H
#define LPC55S69_BOARD_ADS1299_BINDING_H

#include "ads1299_lpc55_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Board-owned ADS1299 HAL factory.
 *
 * This is the application-facing entry point for the LPCXpresso55S69
 * reference integration. Hardware-dependent SPI/GPIO/clock choices remain in
 * board_ads1299_config.h; applications and examples should not duplicate them
 * or edit the shared ADS1299 core.
 *
 * Returns 0 when the board HAL is ready. A negative value means the output
 * pointer, wiring-review interlock, clock assumptions, or lower-level LPC55
 * HAL construction rejected the configuration. No hardware-verification claim
 * is implied by this interface.
 */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

/* Deliberate reference-image safety interlock. Leave zero until the physical
 * wiring has been reviewed against board/pinmap.md. */
extern volatile uint32_t ads1299_board_wiring_reviewed;

#ifdef __cplusplus
}
#endif

#endif /* LPC55S69_BOARD_ADS1299_BINDING_H */
