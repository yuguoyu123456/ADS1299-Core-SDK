#ifndef ADS1299_MIMXRT1166_BOARD_CONFIG_H
#define ADS1299_MIMXRT1166_BOARD_CONFIG_H

/*
 * Single board-dependent configuration contract for MIMXRT1166 integrations.
 *
 * This leaf is not yet tied to a verified reference-board pinout.  Therefore
 * no GPIO numbers, LPSPI instance, or clock-root values are invented here.
 * The consuming MCUXpresso board project defines the ADS1299_BOARD_* macros
 * below from its generated/verified board configuration before including the
 * platform binding.
 *
 * ADS1299 electrical/protocol policy remains in the shared core.  In
 * particular, the platform SPI peripheral must be configured for Mode 1
 * (CPOL=0, CPHA=1), MSB first.
 */

#ifndef ADS1299_BOARD_SPI_BAUD_HZ
#define ADS1299_BOARD_SPI_BAUD_HZ 1000000u
#endif

/* Required integration macros.  Deliberately fail at compile time instead of
 * silently selecting pins that may conflict with boot/debug/peripherals. */
#ifndef ADS1299_BOARD_LPSPI_BASE
#error "Define ADS1299_BOARD_LPSPI_BASE in the MIMXRT1166 board project"
#endif
#ifndef ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ
#error "Define ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ from the configured MCUXpresso clock tree"
#endif

#ifndef ADS1299_BOARD_CS_GPIO
#error "Define ADS1299_BOARD_CS_GPIO"
#endif
#ifndef ADS1299_BOARD_CS_PIN
#error "Define ADS1299_BOARD_CS_PIN"
#endif
#ifndef ADS1299_BOARD_RESET_GPIO
#error "Define ADS1299_BOARD_RESET_GPIO"
#endif
#ifndef ADS1299_BOARD_RESET_PIN
#error "Define ADS1299_BOARD_RESET_PIN"
#endif
#ifndef ADS1299_BOARD_PWDN_GPIO
#error "Define ADS1299_BOARD_PWDN_GPIO"
#endif
#ifndef ADS1299_BOARD_PWDN_PIN
#error "Define ADS1299_BOARD_PWDN_PIN"
#endif
#ifndef ADS1299_BOARD_START_GPIO
#error "Define ADS1299_BOARD_START_GPIO"
#endif
#ifndef ADS1299_BOARD_START_PIN
#error "Define ADS1299_BOARD_START_PIN"
#endif
#ifndef ADS1299_BOARD_DRDY_GPIO
#error "Define ADS1299_BOARD_DRDY_GPIO"
#endif
#ifndef ADS1299_BOARD_DRDY_PIN
#error "Define ADS1299_BOARD_DRDY_PIN"
#endif

#if (ADS1299_BOARD_SPI_BAUD_HZ == 0u)
#error "ADS1299_BOARD_SPI_BAUD_HZ must be non-zero"
#endif
#if (ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ == 0u)
#error "ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ must be non-zero"
#endif

#endif /* ADS1299_MIMXRT1166_BOARD_CONFIG_H */
