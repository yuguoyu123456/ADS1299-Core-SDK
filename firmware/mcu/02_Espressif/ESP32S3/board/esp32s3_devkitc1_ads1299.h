#ifndef ESP32S3_DEVKITC1_ADS1299_H
#define ESP32S3_DEVKITC1_ADS1299_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

/*
 * ESP32-S3-DevKitC-1 reference wiring for ADS1299 bring-up.
 *
 * Keep all board-dependent choices here. Applications should not edit the
 * shared ADS1299 core for a different ESP32-S3 board; change these values
 * instead after checking the target board schematic and pin availability.
 */
#define ADS1299_BOARD_SPI_HOST   SPI2_HOST
#define ADS1299_BOARD_SPI_HZ     (4 * 1000 * 1000)

#define ADS1299_BOARD_PIN_SCLK   GPIO_NUM_12
#define ADS1299_BOARD_PIN_MOSI   GPIO_NUM_11
#define ADS1299_BOARD_PIN_MISO   GPIO_NUM_13
#define ADS1299_BOARD_PIN_CS     GPIO_NUM_10
#define ADS1299_BOARD_PIN_DRDY   GPIO_NUM_9
#define ADS1299_BOARD_PIN_RESET  GPIO_NUM_8
#define ADS1299_BOARD_PIN_START  GPIO_NUM_7
#define ADS1299_BOARD_PIN_PWDN   GPIO_NUM_6

#define ADS1299_BOARD_QUEUE_DEPTH 16u
#define ADS1299_BOARD_DRDY_TIMEOUT_MS 1000u

#endif /* ESP32S3_DEVKITC1_ADS1299_H */
