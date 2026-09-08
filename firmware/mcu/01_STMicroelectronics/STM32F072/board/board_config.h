#ifndef ADS1299_STM32F072_BOARD_CONFIG_H
#define ADS1299_STM32F072_BOARD_CONFIG_H

/* Single board-specific configuration point for the STM32F072 beginner path.
 * Reference board: NUCLEO-F072RB (MB1136, STM32F072RBT6).
 * Configure SPI1 as master/full-duplex/8-bit/MSB-first, CPOL Low, CPHA 2Edge
 * (ADS1299 SPI Mode 1). Keep ADS1299 register/profile logic in shared core.
 *
 * Reference Arduino-header mapping:
 * SCLK PA5/D13, MISO PA6/D12, MOSI PA7/D11, CS PB6/D10,
 * START PB10/D6, PWDN PB4/D5, RESET PB5/D4, DRDY PA10/D2.
 * USART2_TX PA2/D1 may be used for the ST-LINK virtual COM path.
 *
 * In CubeMX label GPIOs ADS1299_CS, ADS1299_RESET, ADS1299_PWDN,
 * ADS1299_START and ADS1299_DRDY. Generated main.h then supplies the
 * matching *_Pin and *_GPIO_Port macros used below.
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

#define ADS1299_BOARD_FRAME_BYTES         27u
#define ADS1299_BOARD_PACKET_BYTES        49u
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES 4u

#endif
