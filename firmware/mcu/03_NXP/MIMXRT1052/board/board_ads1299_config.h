#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

/*
 * MIMXRT1052 ADS1299 board configuration contract.
 *
 * This is the one application-owned configuration point for a new board.
 * Define the macros below in this file or as compiler definitions after
 * selecting the real LPSPI instance, clock root and GPIOs in MCUXpresso.
 * Do not edit the shared ADS1299 core for board bring-up.
 *
 * Reference-board pin assignments are intentionally not guessed here.
 */

#include "fsl_gpio.h"
#include "fsl_lpspi.h"

#ifndef BOARD_ADS1299_LPSPI
#error "Define BOARD_ADS1299_LPSPI to the configured MCUXpresso LPSPI instance"
#endif

#ifndef BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ
#error "Define BOARD_ADS1299_LPSPI_SOURCE_CLOCK_HZ to the actual LPSPI source clock"
#endif

#ifndef BOARD_ADS1299_SPI_BAUD_HZ
#define BOARD_ADS1299_SPI_BAUD_HZ (1000000U)
#endif

#ifndef BOARD_ADS1299_CS_GPIO
#error "Define BOARD_ADS1299_CS_GPIO"
#endif
#ifndef BOARD_ADS1299_CS_PIN
#error "Define BOARD_ADS1299_CS_PIN"
#endif

#ifndef BOARD_ADS1299_RESET_GPIO
#error "Define BOARD_ADS1299_RESET_GPIO"
#endif
#ifndef BOARD_ADS1299_RESET_PIN
#error "Define BOARD_ADS1299_RESET_PIN"
#endif

#ifndef BOARD_ADS1299_PWDN_GPIO
#error "Define BOARD_ADS1299_PWDN_GPIO"
#endif
#ifndef BOARD_ADS1299_PWDN_PIN
#error "Define BOARD_ADS1299_PWDN_PIN"
#endif

#ifndef BOARD_ADS1299_START_GPIO
#error "Define BOARD_ADS1299_START_GPIO"
#endif
#ifndef BOARD_ADS1299_START_PIN
#error "Define BOARD_ADS1299_START_PIN"
#endif

#ifndef BOARD_ADS1299_DRDY_GPIO
#error "Define BOARD_ADS1299_DRDY_GPIO"
#endif
#ifndef BOARD_ADS1299_DRDY_PIN
#error "Define BOARD_ADS1299_DRDY_PIN"
#endif

/* Application must provide a microsecond delay backed by its configured timer. */
#ifndef BOARD_ADS1299_DELAY_US
#error "Define BOARD_ADS1299_DELAY_US to a void function accepting uint32_t microseconds"
#endif

#endif /* BOARD_ADS1299_CONFIG_H */
