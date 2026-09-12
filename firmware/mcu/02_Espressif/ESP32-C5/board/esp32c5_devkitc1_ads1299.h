#ifndef ADS1299_ESP32C5_DEVKITC1_BOARD_H
#define ADS1299_ESP32C5_DEVKITC1_BOARD_H

#include "driver/spi_master.h"

/*
 * Reference board: Espressif ESP32-C5-DevKitC-1 v1.2.
 *
 * This file is the single beginner-editable hardware configuration point for
 * the reference integration.  The selected pins are broken out on the board
 * and intentionally avoid the documented ESP32-C5 strapping pins as well as
 * the native USB pair GPIO13/GPIO14.
 *
 * ADS1299 wiring:
 *   SCLK  -> GPIO6
 *   DIN   -> GPIO8   (MCU MOSI)
 *   DOUT  -> GPIO9   (MCU MISO)
 *   CS    -> GPIO10
 *   DRDY  -> GPIO4
 *   RESET -> GPIO5
 *   PWDN  -> GPIO23
 *   START -> GPIO24
 *
 * Verify the exact DevKitC-1 board revision/header labels before wiring.
 */
#define ADS1299_ESP32C5_PIN_SCLK   6
#define ADS1299_ESP32C5_PIN_MOSI   8
#define ADS1299_ESP32C5_PIN_MISO   9
#define ADS1299_ESP32C5_PIN_CS     10
#define ADS1299_ESP32C5_PIN_DRDY   4
#define ADS1299_ESP32C5_PIN_RESET  5
#define ADS1299_ESP32C5_PIN_PWDN   23
#define ADS1299_ESP32C5_PIN_START  24

/* ADS1299 requires SPI Mode 1 (CPOL=0, CPHA=1), MSB first. */
#define ADS1299_ESP32C5_SPI_HOST   SPI2_HOST
#define ADS1299_ESP32C5_SPI_MODE   1
#define ADS1299_ESP32C5_SPI_HZ     4000000

#endif
