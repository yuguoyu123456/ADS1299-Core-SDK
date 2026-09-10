#ifndef ADS1299_STM32H563_BOARD_CONFIG_H
#define ADS1299_STM32H563_BOARD_CONFIG_H

/*
 * Reference board: NUCLEO-H563ZI (MB1404, STM32H563ZIT6)
 * Validation status: TEMPLATE / integration configuration present.
 *
 * Beginner rule: board-specific pins, peripherals, clocks and transport
 * choices belong here. Do not edit the shared ADS1299 core for bring-up.
 */

#define ADS1299_BOARD_NAME               "NUCLEO-H563ZI"
#define ADS1299_BOARD_MCU                "STM32H563ZIT6"
#define ADS1299_BOARD_REFERENCE          "MB1404"

/*
 * ADS1299 digital bus: SPI mode 1 (CPOL=0, CPHA=1), 8-bit, MSB first.
 * NUCLEO-H563ZI uses the SPI1 routing recommended by ST for a path that does
 * not require removing the PA7-related solder bridge: PA5 / PG9 / PB5.
 */
#define ADS1299_BOARD_SPI_INSTANCE        1u
#define ADS1299_BOARD_SPI_SCK_PORT        'A'
#define ADS1299_BOARD_SPI_SCK_PIN         5u   /* SPI1_SCK, Zio D13 */
#define ADS1299_BOARD_SPI_MISO_PORT       'G'
#define ADS1299_BOARD_SPI_MISO_PIN        9u   /* SPI1_MISO, Zio D12 */
#define ADS1299_BOARD_SPI_MOSI_PORT       'B'
#define ADS1299_BOARD_SPI_MOSI_PIN        5u   /* SPI1_MOSI, Zio D11 */
#define ADS1299_BOARD_SPI_CPOL            0u
#define ADS1299_BOARD_SPI_CPHA            1u
#define ADS1299_BOARD_SPI_MSB_FIRST       1u

/* One-ADS1299 starter control wiring on exposed Zio GPIOs. */
#define ADS1299_BOARD_CS_PORT             'D'
#define ADS1299_BOARD_CS_PIN              14u  /* Zio D10 */
#define ADS1299_BOARD_DRDY_PORT           'D'
#define ADS1299_BOARD_DRDY_PIN            15u  /* Zio D9, active low */
#define ADS1299_BOARD_RESET_PORT          'F'
#define ADS1299_BOARD_RESET_PIN           3u   /* Zio D8, active low */
#define ADS1299_BOARD_PWDN_PORT           'G'
#define ADS1299_BOARD_PWDN_PIN            2u   /* Zio D49, active low */
#define ADS1299_BOARD_START_PORT          'G'
#define ADS1299_BOARD_START_PIN           3u   /* Zio D50 */

/* Default host debug/stream transport: ST-LINK virtual COM port. */
#define ADS1299_BOARD_HOST_UART_INSTANCE  3u
#define ADS1299_BOARD_HOST_TX_PORT        'D'
#define ADS1299_BOARD_HOST_TX_PIN         8u   /* USART3_TX / ST-LINK VCP */
#define ADS1299_BOARD_HOST_RX_PORT        'D'
#define ADS1299_BOARD_HOST_RX_PIN         9u   /* USART3_RX / ST-LINK VCP */
#define ADS1299_BOARD_HOST_BAUD           460800u

/* Canonical beginner acquisition defaults. */
#define ADS1299_BOARD_DEFAULT_SPS         250u
#define ADS1299_BOARD_DEFAULT_GAIN        24u
#define ADS1299_BOARD_DRDY_TIMEOUT_MS     20u
#define ADS1299_BOARD_RAW_FRAME_BYTES     27u
#define ADS1299_BOARD_PACKET_BYTES        49u

/*
 * Keep the DRDY path bounded. For sustained acquisition, capture/read
 * promptly, enqueue into static storage, and packetize/transport outside the
 * timing-critical ISR. Overflow must be counted rather than hidden.
 */
#define ADS1299_BOARD_FRAME_QUEUE_DEPTH   16u

/*
 * STM32CubeH5 HAL aliases for the reference-board path. Include generated
 * main.h / stm32h5xx_hal.h before this header so GPIOx and GPIO_PIN_x names
 * are available. A custom STM32H563 board should normally change only this
 * board file plus CubeMX peripheral/pin generation.
 */
#if defined(GPIOA) && defined(GPIOB) && defined(GPIOD) && defined(GPIOF) && defined(GPIOG)
#define ADS1299_H563_CS_GPIO_PORT         GPIOD
#define ADS1299_H563_CS_GPIO_PIN          GPIO_PIN_14
#define ADS1299_H563_DRDY_GPIO_PORT       GPIOD
#define ADS1299_H563_DRDY_GPIO_PIN        GPIO_PIN_15
#define ADS1299_H563_RESET_GPIO_PORT      GPIOF
#define ADS1299_H563_RESET_GPIO_PIN       GPIO_PIN_3
#define ADS1299_H563_PWDN_GPIO_PORT       GPIOG
#define ADS1299_H563_PWDN_GPIO_PIN        GPIO_PIN_2
#define ADS1299_H563_START_GPIO_PORT      GPIOG
#define ADS1299_H563_START_GPIO_PIN       GPIO_PIN_3
#endif

#endif /* ADS1299_STM32H563_BOARD_CONFIG_H */
