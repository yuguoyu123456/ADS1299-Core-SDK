#ifndef ESP8684_DEVKITM1_ADS1299_H
#define ESP8684_DEVKITM1_ADS1299_H

/*
 * Reference board: Espressif ESP8684-DevKitM-1 v1.1 (ESP32-C2 family).
 *
 * This is the single hardware-dependent file for the reference example.
 * Change these values when using another ESP32-C2 board.
 *
 * The default assignment deliberately avoids GPIO8/GPIO9 strapping pins and
 * UART0 GPIO19/GPIO20.  GPIO0/GPIO1 are also left unused because the v1.1
 * board drives RGB LED channels from them.
 */

#define ADS1299_ESP32C2_SPI_HOST      SPI2_HOST
#define ADS1299_ESP32C2_SPI_HZ        4000000

#define ADS1299_ESP32C2_PIN_MISO      2
#define ADS1299_ESP32C2_PIN_MOSI      7
#define ADS1299_ESP32C2_PIN_SCLK      6
#define ADS1299_ESP32C2_PIN_CS        10
#define ADS1299_ESP32C2_PIN_DRDY      3
#define ADS1299_ESP32C2_PIN_RESET     4
#define ADS1299_ESP32C2_PIN_PWDN      5
#define ADS1299_ESP32C2_PIN_START     18

/* ADS1299 uses SPI mode 1: CPOL=0, CPHA=1. */
#define ADS1299_ESP32C2_SPI_MODE      1

#endif
