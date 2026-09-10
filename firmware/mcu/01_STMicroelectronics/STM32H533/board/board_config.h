#ifndef ADS1299_STM32H533_BOARD_CONFIG_H
#define ADS1299_STM32H533_BOARD_CONFIG_H

/*
 * Reference board: NUCLEO-H533RE (MB1814, STM32H533RET6)
 * Validation status: TEMPLATE / integration configuration present.
 *
 * Beginner rule: board-specific pins, peripherals, clocks and transport
 * choices belong here. Do not edit the shared ADS1299 core for bring-up.
 */

#define ADS1299_BOARD_NAME              "NUCLEO-H533RE"
#define ADS1299_BOARD_MCU               "STM32H533RET6"
#define ADS1299_BOARD_REFERENCE          "MB1814"

/* ADS1299 digital bus: SPI mode 1 (CPOL=0, CPHA=1), 8-bit, MSB first. */
#define ADS1299_BOARD_SPI_INSTANCE       1u
#define ADS1299_BOARD_SPI_SCK_PORT       'A'
#define ADS1299_BOARD_SPI_SCK_PIN        5u   /* Arduino D13 / SPI1_SCK */
#define ADS1299_BOARD_SPI_MISO_PORT      'A'
#define ADS1299_BOARD_SPI_MISO_PIN       6u   /* Arduino D12 / SPI1_MISO */
#define ADS1299_BOARD_SPI_MOSI_PORT      'A'
#define ADS1299_BOARD_SPI_MOSI_PIN       7u   /* Arduino D11 / SPI1_MOSI */
#define ADS1299_BOARD_SPI_CPOL           0u
#define ADS1299_BOARD_SPI_CPHA           1u
#define ADS1299_BOARD_SPI_MSB_FIRST      1u

/* One-ADS1299 starter control wiring. */
#define ADS1299_BOARD_CS_PORT            'C'
#define ADS1299_BOARD_CS_PIN             9u   /* Arduino D10 */
#define ADS1299_BOARD_DRDY_PORT          'C'
#define ADS1299_BOARD_DRDY_PIN           6u   /* Arduino D9, active low */
#define ADS1299_BOARD_RESET_PORT         'C'
#define ADS1299_BOARD_RESET_PIN          7u   /* Arduino D8, active low */
#define ADS1299_BOARD_PWDN_PORT          'A'
#define ADS1299_BOARD_PWDN_PIN           8u   /* Arduino D7, active low */
#define ADS1299_BOARD_START_PORT         'B'
#define ADS1299_BOARD_START_PIN          10u  /* Arduino D6 */

/* Default host debug/stream transport: STLINK-V3EC virtual COM port. */
#define ADS1299_BOARD_HOST_UART_INSTANCE 2u
#define ADS1299_BOARD_HOST_TX_PORT       'A'
#define ADS1299_BOARD_HOST_TX_PIN        2u   /* USART2_TX / STLINK VCP */
#define ADS1299_BOARD_HOST_RX_PORT       'A'
#define ADS1299_BOARD_HOST_RX_PIN        3u   /* USART2_RX / STLINK VCP */
#define ADS1299_BOARD_HOST_BAUD          460800u

/* Canonical beginner acquisition defaults. */
#define ADS1299_BOARD_DEFAULT_SPS        250u
#define ADS1299_BOARD_DEFAULT_GAIN       24u
#define ADS1299_BOARD_DRDY_TIMEOUT_MS    20u
#define ADS1299_BOARD_RAW_FRAME_BYTES    27u
#define ADS1299_BOARD_PACKET_BYTES       49u

/*
 * H533 has enough SRAM for a modest static queue. Keep the DRDY path short:
 * latch/read promptly, enqueue bounded work, and packetize/transport outside
 * the timing-critical ISR. Overflow must be counted rather than hidden.
 */
#define ADS1299_BOARD_FRAME_QUEUE_DEPTH  16u

/*
 * STM32CubeH5 HAL aliases for the reference-board path. Include the generated
 * main.h / stm32h5xx_hal.h before this header so GPIOA/B/C and GPIO_PIN_x are
 * available. A custom STM32H533 board normally changes only this board file
 * plus CubeMX peripheral/pin generation; the shared ADS1299 core stays intact.
 */
#if defined(GPIOA) && defined(GPIOB) && defined(GPIOC)
#define ADS1299_H533_CS_GPIO_PORT        GPIOC
#define ADS1299_H533_CS_GPIO_PIN         GPIO_PIN_9
#define ADS1299_H533_DRDY_GPIO_PORT      GPIOC
#define ADS1299_H533_DRDY_GPIO_PIN       GPIO_PIN_6
#define ADS1299_H533_RESET_GPIO_PORT     GPIOC
#define ADS1299_H533_RESET_GPIO_PIN      GPIO_PIN_7
#define ADS1299_H533_PWDN_GPIO_PORT      GPIOA
#define ADS1299_H533_PWDN_GPIO_PIN       GPIO_PIN_8
#define ADS1299_H533_START_GPIO_PORT     GPIOB
#define ADS1299_H533_START_GPIO_PIN      GPIO_PIN_10
#endif

#endif /* ADS1299_STM32H533_BOARD_CONFIG_H */
