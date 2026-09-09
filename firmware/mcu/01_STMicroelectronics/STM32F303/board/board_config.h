#ifndef ADS1299_STM32F303_BOARD_CONFIG_H
#define ADS1299_STM32F303_BOARD_CONFIG_H

/* Single board-specific configuration point for the STM32F303 beginner path.
 * Reference board: NUCLEO-F303RE (MB1136, STM32F303RET6).
 *
 * Configure SPI1 as master, full duplex, 8-bit, MSB-first, CPOL Low,
 * CPHA 2Edge. This is ADS1299 SPI Mode 1 (CPOL=0, CPHA=1).
 * ADS1299 register/profile behavior belongs in the shared core and must not be
 * changed when board pins or peripheral instances change.
 *
 * NUCLEO-F303RE Arduino-header mapping used by this reference configuration:
 *   SCLK  PA5  / D13
 *   MISO  PA6  / D12   <- ADS1299 DOUT
 *   MOSI  PA7  / D11   -> ADS1299 DIN
 *   CS    PB6  / D10
 *   START PB10 / D6
 *   PWDN  PB4  / D5
 *   RESET PB5  / D4
 *   DRDY  PA10 / D2
 *   TX    PA2  / D1    optional USART2 host stream
 *
 * In STM32CubeMX label GPIOs ADS1299_CS, ADS1299_RESET, ADS1299_PWDN,
 * ADS1299_START and ADS1299_DRDY. Generated main.h then supplies the matching
 * *_Pin and *_GPIO_Port macros referenced below.
 *
 * Note: PA2/USART2 is routed to the on-board ST-LINK virtual COM path on the
 * standard Nucleo configuration. If the physical Arduino D1 pin is required
 * for another purpose, choose another UART/pin pair in CubeMX and change only
 * ADS1299_BOARD_UART_HANDLE plus the transport wiring.
 *
 * When moving to another STM32F303 board, keep the shared ADS1299 core intact:
 * change CubeMX pin/peripheral routing and, if needed, only the handle macros
 * below.
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

/* Canonical ADS1299-8 and repository packet sizing. */
#define ADS1299_BOARD_FRAME_BYTES         27u
#define ADS1299_BOARD_PACKET_BYTES        49u

/* Starter path stays bounded/static. Higher-rate or multi-device applications
 * should use the platform buffering/DMA strategy documented at model level. */
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES 8u

#endif
