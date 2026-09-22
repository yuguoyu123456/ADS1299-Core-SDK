#ifndef ADS1299_LPC55S69_BOARD_CONFIG_H
#define ADS1299_LPC55S69_BOARD_CONFIG_H

/*
 * Single board-local configuration point for LPC55S69 ADS1299 bring-up.
 *
 * This header intentionally contains no ADS1299 register policy.  The shared
 * ADS1299 core owns device configuration; this file only describes the board
 * binding used by board/reference_image.c.
 *
 * The defaults reproduce the existing LPCXpresso55S69 reference image.  Keep
 * ADS1299_BOARD_WIRING_REVIEW_REQUIRED enabled until the physical wiring has
 * been checked.  A different LPC55S69 board should normally change this file
 * (and its MCUXpresso pin mux, if required), not the shared ADS1299 core.
 */

#define ADS1299_BOARD_NAME "NXP LPCXpresso55S69"

#define ADS1299_BOARD_SPI_FLEXCOMM_INDEX 7u
#define ADS1299_BOARD_SPI_SOURCE_HZ      12000000u
#define ADS1299_BOARD_SPI_HZ             1000000u
#define ADS1299_BOARD_SPI_POLL_LIMIT     100000u

/* GPIO controls used by the existing ads1299_lpc55_context_t order:
 * CS, RESET, PWDN, START, DRDY.
 */
#define ADS1299_BOARD_CS_PORT       1u
#define ADS1299_BOARD_CS_PIN        31u
#define ADS1299_BOARD_RESET_PORT    0u
#define ADS1299_BOARD_RESET_PIN     26u
#define ADS1299_BOARD_PWDN_PORT     0u
#define ADS1299_BOARD_PWDN_PIN      27u
#define ADS1299_BOARD_START_PORT    1u
#define ADS1299_BOARD_START_PIN     6u
#define ADS1299_BOARD_DRDY_PORT     1u
#define ADS1299_BOARD_DRDY_PIN      7u

/* Existing reference image uses P0_19..P0_21 with IOCON function 7 for SPI7.
 * Keep the three pads explicit so a custom LPC55S69 board has one obvious
 * place to review its SCK/MOSI/MISO routing.
 */
#define ADS1299_BOARD_SPI_PAD_PORT  0u
#define ADS1299_BOARD_SPI_PAD0_PIN  19u
#define ADS1299_BOARD_SPI_PAD1_PIN  20u
#define ADS1299_BOARD_SPI_PAD2_PIN  21u
#define ADS1299_BOARD_SPI_IOCON_FUNC 7u

/* Safety interlock policy.  The reference image also requires the runtime
 * ads1299_board_wiring_reviewed flag to equal 1 before it drives the board.
 */
#define ADS1299_BOARD_WIRING_REVIEW_REQUIRED 1u

#endif /* ADS1299_LPC55S69_BOARD_CONFIG_H */
