#ifndef ESP32C3_DEVKITM1_ADS1299_H
#define ESP32C3_DEVKITM1_ADS1299_H

/*
 * Reference board: Espressif ESP32-C3-DevKitM-1.
 * This is the single hardware-dependent file for the reference example.
 * Change these values when using another ESP32-C3 board.
 *
 * Defaults avoid the GPIO2/GPIO8/GPIO9 strapping pins, GPIO8 RGB LED,
 * USB Serial/JTAG GPIO18/GPIO19, and UART0 GPIO20/GPIO21.
 */

#define ADS1299_ESP32C3_SPI_HOST      SPI2_HOST
#define ADS1299_ESP32C3_SPI_HZ        4000000
#define ADS1299_ESP32C3_SPI_MODE      1

#define ADS1299_ESP32C3_PIN_MISO      3
#define ADS1299_ESP32C3_PIN_MOSI      7
#define ADS1299_ESP32C3_PIN_SCLK      6
#define ADS1299_ESP32C3_PIN_CS        10
#define ADS1299_ESP32C3_PIN_DRDY      4
#define ADS1299_ESP32C3_PIN_RESET     5
#define ADS1299_ESP32C3_PIN_PWDN      0
#define ADS1299_ESP32C3_PIN_START     1

#endif
