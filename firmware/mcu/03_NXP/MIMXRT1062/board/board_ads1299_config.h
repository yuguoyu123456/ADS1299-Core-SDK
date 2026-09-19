#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

/*
 * MIMXRT1062 ADS1299 board configuration contract.
 *
 * This is the one intended hardware-dependent edit point for a new RT1062
 * board.  The shared ADS1299 core must not be edited for pin/peripheral
 * changes.  The EVKB reference binding remains in evkb_reference.c until it
 * is migrated to consume this contract.
 *
 * Validation status: TEMPLATE.  Values below describe the repository's
 * existing EVKB reference binding; confirm board/pinmap.md and actual wiring
 * before hardware use.  This file does not imply build- or board-verification.
 */

#include "fsl_gpio.h"
#include "fsl_lpspi.h"

#define BOARD_ADS1299_LPSPI              LPSPI1
#define BOARD_ADS1299_SPI_BAUD_HZ        1000000u

#define BOARD_ADS1299_CS_GPIO            GPIO3
#define BOARD_ADS1299_CS_PIN             13u
#define BOARD_ADS1299_RESET_GPIO         GPIO1
#define BOARD_ADS1299_RESET_PIN          24u
#define BOARD_ADS1299_PWDN_GPIO          GPIO1
#define BOARD_ADS1299_PWDN_PIN           9u
#define BOARD_ADS1299_START_GPIO         GPIO1
#define BOARD_ADS1299_START_PIN          10u
#define BOARD_ADS1299_DRDY_GPIO          GPIO1
#define BOARD_ADS1299_DRDY_PIN           11u

/*
 * Deliberately no guessed fixed source-clock value here.  MCUXpresso board
 * code must obtain the active LPSPI root clock after clock initialization,
 * e.g. CLOCK_GetClockRootFreq(kCLOCK_LpspiClkRoot), and pass it to the
 * adapter.  This keeps clock-tree policy in the vendor board layer.
 */

#if (BOARD_ADS1299_SPI_BAUD_HZ == 0u)
#error "BOARD_ADS1299_SPI_BAUD_HZ must be non-zero"
#endif

#endif /* BOARD_ADS1299_CONFIG_H */
