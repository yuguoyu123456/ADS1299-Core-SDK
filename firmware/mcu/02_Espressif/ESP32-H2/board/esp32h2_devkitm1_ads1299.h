#ifndef ADS1299_ESP32H2_DEVKITM1_BOARD_H
#define ADS1299_ESP32H2_DEVKITM1_BOARD_H

#include "driver/spi_master.h"

/*
 * Reference board: Espressif ESP32-H2-DevKitM-1.
 * Single beginner-editable hardware configuration point.
 *
 * The selected pins are broken out on the official board and avoid GPIO9
 * (BOOT), GPIO8 (module RGB LED), GPIO26/GPIO27 (USB), GPIO23/GPIO24
 * (UART0), and optional 32-kHz crystal pins GPIO13/GPIO14.
 *
 * ADS1299 wiring:
 *   SCLK  -> GPIO4
 *   DIN   -> GPIO5   (MCU MOSI)
 *   DOUT  -> GPIO2   (MCU MISO)
 *   CS    -> GPIO10
 *   DRDY  -> GPIO11
 *   RESET -> GPIO12
 *   PWDN  -> GPIO22
 *   START -> GPIO25
 */
#define ADS1299_ESP32H2_PIN_SCLK   4
#define ADS1299_ESP32H2_PIN_MOSI   5
#define ADS1299_ESP32H2_PIN_MISO   2
#define ADS1299_ESP32H2_PIN_CS     10
#define ADS1299_ESP32H2_PIN_DRDY   11
#define ADS1299_ESP32H2_PIN_RESET  12
#define ADS1299_ESP32H2_PIN_PWDN   22
#define ADS1299_ESP32H2_PIN_START  25

/* ADS1299 requires SPI Mode 1 (CPOL=0, CPHA=1), MSB first. */
#define ADS1299_ESP32H2_SPI_HOST   SPI2_HOST
#define ADS1299_ESP32H2_SPI_MODE   1
#define ADS1299_ESP32H2_SPI_HZ     4000000

#endif
