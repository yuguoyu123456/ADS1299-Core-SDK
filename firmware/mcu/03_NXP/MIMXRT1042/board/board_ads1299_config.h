#ifndef BOARD_ADS1299_CONFIG_H
#define BOARD_ADS1299_CONFIG_H

/*
 * MIMXRT1042 ADS1299 board-only configuration contract.
 *
 * TEMPLATE / NOT BUILD-VERIFIED / NOT BOARD-VERIFIED.
 *
 * This is the one obvious hardware-dependent configuration surface for a
 * consuming MCUXpresso application. Define the BOARD_ADS1299_* symbols in the
 * application's build flags or replace the #error entries below with symbols
 * from its generated pin/clock configuration. Do not edit the shared ADS1299
 * core for board bring-up.
 */

#ifndef BOARD_ADS1299_LPSPI
#error "BOARD_ADS1299_LPSPI: bind the MCUXpresso LPSPI instance used by ADS1299"
#endif

#ifndef BOARD_ADS1299_LPSPI_CLOCK_HZ
#error "BOARD_ADS1299_LPSPI_CLOCK_HZ: bind the actual LPSPI source clock in Hz"
#endif

#ifndef BOARD_ADS1299_SPI_BAUD_HZ
#define BOARD_ADS1299_SPI_BAUD_HZ 1000000u
#endif

#ifndef BOARD_ADS1299_GPIO
#error "BOARD_ADS1299_GPIO: bind the GPIO peripheral used by ADS1299 control pins"
#endif

#ifndef BOARD_ADS1299_CS_PIN
#error "BOARD_ADS1299_CS_PIN: bind software-controlled chip select"
#endif

#ifndef BOARD_ADS1299_RESET_PIN
#error "BOARD_ADS1299_RESET_PIN: bind ADS1299 RESET"
#endif

#ifndef BOARD_ADS1299_PWDN_PIN
#error "BOARD_ADS1299_PWDN_PIN: bind ADS1299 PWDN"
#endif

#ifndef BOARD_ADS1299_START_PIN
#error "BOARD_ADS1299_START_PIN: bind ADS1299 START"
#endif

#ifndef BOARD_ADS1299_DRDY_PIN
#error "BOARD_ADS1299_DRDY_PIN: bind ADS1299 DRDY input"
#endif

/*
 * board_ads1299_delay_us() is intentionally application-owned so the SDK does
 * not invent a timer implementation. It must provide a bring-up-safe
 * microsecond delay using the consuming project's real timebase.
 */

#endif /* BOARD_ADS1299_CONFIG_H */
