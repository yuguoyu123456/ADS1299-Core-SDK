#ifndef ADS1299_STM32F767_BOARD_CONFIG_H
#define ADS1299_STM32F767_BOARD_CONFIG_H

/*
 * Reference starter board: ST NUCLEO-F767ZI (MB1137, STM32F767ZIT6).
 *
 * This is the single repository-owned configuration point for the reference
 * board. Keep ADS1299 register/profile logic in firmware/core_driver/ads1299.
 * When porting to another STM32F767 board, change CubeMX routing and these
 * board-level selections rather than editing the shared ADS1299 core.
 */

/* STM32Cube/HAL peripheral selections used by the starter integration. */
#define ADS1299_BOARD_SPI_INSTANCE              SPI1
#define ADS1299_BOARD_STREAM_USART_INSTANCE     USART3

/* NUCLEO-F767ZI / Arduino-compatible header starter routing. */
#define ADS1299_BOARD_SCK_GPIO_PORT              GPIOA
#define ADS1299_BOARD_SCK_PIN                    GPIO_PIN_5   /* CN7 D13, SPI1_SCK */
#define ADS1299_BOARD_MISO_GPIO_PORT             GPIOA
#define ADS1299_BOARD_MISO_PIN                   GPIO_PIN_6   /* CN7 D12, SPI1_MISO */
#define ADS1299_BOARD_MOSI_GPIO_PORT             GPIOA
#define ADS1299_BOARD_MOSI_PIN                   GPIO_PIN_7   /* CN7 D11, SPI1_MOSI */

#define ADS1299_BOARD_CS_GPIO_PORT               GPIOD
#define ADS1299_BOARD_CS_PIN                     GPIO_PIN_14  /* CN7 D10 */
#define ADS1299_BOARD_DRDY_GPIO_PORT             GPIOD
#define ADS1299_BOARD_DRDY_PIN                   GPIO_PIN_15  /* CN7 D9 */
#define ADS1299_BOARD_RESET_GPIO_PORT            GPIOF
#define ADS1299_BOARD_RESET_PIN                  GPIO_PIN_12  /* CN7 D8 */
#define ADS1299_BOARD_PWDN_GPIO_PORT             GPIOF
#define ADS1299_BOARD_PWDN_PIN                   GPIO_PIN_13  /* CN10 D7 */
#define ADS1299_BOARD_START_GPIO_PORT            GPIOE
#define ADS1299_BOARD_START_PIN                  GPIO_PIN_9   /* CN10 D6 */

/* ST-LINK virtual COM port on the default NUCLEO-F767ZI USART3 routing. */
#define ADS1299_BOARD_STREAM_TX_GPIO_PORT         GPIOD
#define ADS1299_BOARD_STREAM_TX_PIN               GPIO_PIN_8  /* USART3_TX */
#define ADS1299_BOARD_STREAM_RX_GPIO_PORT         GPIOD
#define ADS1299_BOARD_STREAM_RX_PIN               GPIO_PIN_9  /* USART3_RX */
#define ADS1299_BOARD_STREAM_BAUD                 115200u

/* ADS1299 transport invariants for the shared driver. */
#define ADS1299_BOARD_SPI_CPOL                    0u
#define ADS1299_BOARD_SPI_CPHA                    1u
#define ADS1299_BOARD_SPI_BITS                    8u
#define ADS1299_BOARD_SPI_MSB_FIRST               1u
#define ADS1299_BOARD_RAW_FRAME_BYTES             27u
#define ADS1299_BOARD_CANONICAL_PACKET_BYTES      49u

/* Beginner profile defaults. */
#define ADS1299_BOARD_BEGINNER_SAMPLE_RATE_SPS    250u
#define ADS1299_BOARD_BEGINNER_GAIN               24u
#define ADS1299_BOARD_DRDY_TIMEOUT_MS              100u

/*
 * Starter buffering contract. This is intentionally finite so overflow is
 * visible. Sustained/high-rate and multi-device applications should use
 * interrupt/DMA acquisition into a bounded queue/ring and count overflows.
 */
#define ADS1299_BOARD_STARTER_QUEUE_FRAMES        16u

#endif /* ADS1299_STM32F767_BOARD_CONFIG_H */
