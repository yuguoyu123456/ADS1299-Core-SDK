#ifndef MIMXRT1189_BOARD_ADS1299_H
#define MIMXRT1189_BOARD_ADS1299_H

#include "ads1299_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Single board-dependent entry point for the MIMXRT1189 ADS1299 examples.
 *
 * Implement this function in the consuming official MCUXpresso board project.
 * That implementation owns LPSPI selection/clocking, pin mux, GPIO numbers,
 * delays and optional timestamp plumbing. It must not contain ADS1299 register
 * programming; register/device policy belongs to firmware/core_driver/ads1299.
 *
 * Required electrical/protocol contract for the SPI callback:
 *   - SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
 *   - CS is software-controlled through the separate GPIO callback.
 *   - spi_transfer(NULL, rx, n) clocks zero bytes while receiving.
 *   - spi_transfer(tx, NULL, n) discards received bytes.
 *
 * Required signals are CS, DRDY, START, RESET and PWDN. The board project is
 * responsible for configuring their mux/direction before returning success.
 * Return 0 only when every callback/context required by ads1299_mcu_port_init()
 * is ready; return nonzero for board/peripheral configuration failure.
 *
 * This header is a software integration contract only. It does not claim a
 * verified MIMXRT1189-EVK pin mapping or physical-board validation.
 */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

#ifdef __cplusplus
}
#endif

#endif /* MIMXRT1189_BOARD_ADS1299_H */
