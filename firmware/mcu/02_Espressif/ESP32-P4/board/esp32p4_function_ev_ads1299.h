#ifndef ADS1299_ESP32P4_FUNCTION_EV_BOARD_H
#define ADS1299_ESP32P4_FUNCTION_EV_BOARD_H

#include "driver/spi_master.h"

/*
 * Reference board: Espressif ESP32-P4-Function-EV-Board v1.4.
 * Single beginner-editable hardware configuration point.
 *
 * All selected signals are broken out on J1. GPIO37/GPIO38 are intentionally
 * avoided because the board documents them as UART0 TX/RX. GPIO0/GPIO1 and
 * GPIO45 are also avoided because the official board documentation notes
 * hardware-option constraints for those pins.
 *
 * ADS1299 wiring:
 *   SCLK  -> GPIO4
 *   DIN   -> GPIO5   (MCU MOSI)
 *   DOUT  -> GPIO3   (MCU MISO)
 *   CS    -> GPIO7
 *   DRDY  -> GPIO8
 *   RESET -> GPIO20
 *   PWDN  -> GPIO21
 *   START -> GPIO22
 */
#define ADS1299_ESP32P4_PIN_SCLK   4
#define ADS1299_ESP32P4_PIN_MOSI   5
#define ADS1299_ESP32P4_PIN_MISO   3
#define ADS1299_ESP32P4_PIN_CS     7
#define ADS1299_ESP32P4_PIN_DRDY   8
#define ADS1299_ESP32P4_PIN_RESET  20
#define ADS1299_ESP32P4_PIN_PWDN   21
#define ADS1299_ESP32P4_PIN_START  22

/* ADS1299 requires SPI Mode 1 (CPOL=0, CPHA=1), MSB first. */
#define ADS1299_ESP32P4_SPI_HOST   SPI2_HOST
#define ADS1299_ESP32P4_SPI_MODE   1
#define ADS1299_ESP32P4_SPI_HZ     4000000

#endif
