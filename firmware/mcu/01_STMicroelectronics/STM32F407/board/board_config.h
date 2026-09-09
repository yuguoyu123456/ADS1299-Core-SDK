#ifndef ADS1299_STM32F407_BOARD_CONFIG_H
#define ADS1299_STM32F407_BOARD_CONFIG_H

/* Single board-specific configuration point for the STM32F407 beginner path.
 * Reference board: STM32F407G-DISC1 (MB997, STM32F407VGT6).
 *
 * Configure SPI1 as master, full duplex, 8-bit, MSB-first, CPOL Low,
 * CPHA 2Edge. This is ADS1299 SPI Mode 1 (CPOL=0, CPHA=1).
 * Shared ADS1299 register/profile behavior stays in firmware/core_driver.
 *
 * Reference MCU signals:
 *   SCLK   PA5   SPI1_SCK
 *   MISO   PA6   SPI1_MISO <- ADS1299 DOUT
 *   MOSI   PA7   SPI1_MOSI -> ADS1299 DIN
 *   CS     PB0
 *   DRDY   PB1
 *   RESET  PB11
 *   PWDN   PB12
 *   START  PB13
 *   TX     PA2   optional host transport
 *
 * IMPORTANT: PA5/PA6/PA7 are also connected to on-board Discovery peripherals
 * on MB997 revisions. Verify solder bridges/on-board peripheral loading against
 * the exact MB997 schematic before attaching ADS1299. The beginner software
 * contract is stable even if another free pin/peripheral mapping is selected.
 *
 * In STM32CubeMX label GPIOs ADS1299_CS, ADS1299_RESET, ADS1299_PWDN,
 * ADS1299_START and ADS1299_DRDY. Generated main.h then supplies the matching
 * *_Pin and *_GPIO_Port macros referenced below.
 *
 * When moving to another STM32F407 board, change CubeMX pin/peripheral routing
 * and, if needed, only the handle macros below. Do not edit the shared core.
 */

#define ADS1299_BOARD_SPI_HANDLE          hspi1
#define ADS1299_BOARD_USE_UART_STREAM     1
#define ADS1299_BOARD_UART_HANDLE         huart2

#define ADS1299_BOARD_CS_GPIO_PORT        ADS1299_CS_GPIO_Port
#define ADS1299_BOARD_CS_PIN              ADS1299_CS_Pin
#define ADS1299_BOARD_RESET_GPIO_PORT     ADS1299_RESET_GPIO_Port
#define ADS1299_BOARD_RESET_PIN           ADS1299_RESET_Pin
#define ADS1299_BOARD_PWDN_GPIO_PORT      ADS1299_PWDN_GPIO_Port
#define ADS1299_BOARD_PWDN_PIN            ADS1299_PWDN_Pin
#define ADS1299_BOARD_START_GPIO_PORT     ADS1299_START_GPIO_Port
#define ADS1299_BOARD_START_PIN           ADS1299_START_Pin
#define ADS1299_BOARD_DRDY_GPIO_PORT      ADS1299_DRDY_GPIO_Port
#define ADS1299_BOARD_DRDY_PIN            ADS1299_DRDY_Pin

#define ADS1299_BOARD_SPI_MODE            1u
#define ADS1299_BOARD_SPI_MSB_FIRST       1u
#define ADS1299_BOARD_DRDY_ACTIVE_LOW     1u
#define ADS1299_BOARD_CS_ACTIVE_LOW       1u
#define ADS1299_BOARD_RESET_ACTIVE_LOW    1u
#define ADS1299_BOARD_PWDN_ACTIVE_LOW     1u

/* ADS1299-8 continuous frame and canonical repository packet sizes. */
#define ADS1299_BOARD_FRAME_BYTES         27u
#define ADS1299_BOARD_PACKET_BYTES        49u

/* Bounded starter queue. High-rate/multi-device applications should move
 * capture out of blocking transport paths and use DMA plus bounded buffering. */
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES 16u

#endif
