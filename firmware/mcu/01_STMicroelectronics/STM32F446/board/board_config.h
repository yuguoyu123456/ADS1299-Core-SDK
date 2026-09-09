#ifndef ADS1299_STM32F446_BOARD_CONFIG_H
#define ADS1299_STM32F446_BOARD_CONFIG_H

/*
 * ADS1299-Core-SDK STM32F446 reference-board configuration
 *
 * Reference board : NUCLEO-F446RE (MB1136)
 * MCU             : STM32F446RET6
 * Toolchain       : STM32CubeMX / STM32CubeIDE + STM32CubeF4 HAL
 *
 * This is the single obvious repository-owned hardware configuration point
 * for the beginner path.  When moving to another STM32F446 board, change the
 * CubeMX routing and this board layer; do not edit shared ADS1299 register,
 * model, frame or packet logic.
 */

/* ADS1299 SPI contract: Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first. */
#define ADS1299_BOARD_SPI_INSTANCE              SPI1
#define ADS1299_BOARD_SPI_CPOL                  0u
#define ADS1299_BOARD_SPI_CPHA                  1u
#define ADS1299_BOARD_SPI_MSB_FIRST             1u

/*
 * NUCLEO-F446RE Arduino-header SPI1 routing from ST UM1724:
 * D13=PA5/SPI1_SCK, D12=PA6/SPI1_MISO, D11=PA7/SPI1_MOSI.
 */
#define ADS1299_BOARD_SCK_GPIO_PORT             GPIOA
#define ADS1299_BOARD_SCK_GPIO_PIN              GPIO_PIN_5
#define ADS1299_BOARD_MISO_GPIO_PORT            GPIOA
#define ADS1299_BOARD_MISO_GPIO_PIN              GPIO_PIN_6
#define ADS1299_BOARD_MOSI_GPIO_PORT            GPIOA
#define ADS1299_BOARD_MOSI_GPIO_PIN              GPIO_PIN_7

/*
 * Starter control routing uses ordinary header-accessible GPIOs.  Keep CS
 * software-controlled so shared-SPI + independent-CS multi-ADS1299 designs
 * remain possible without changing the core driver architecture.
 */
#define ADS1299_BOARD_CS_GPIO_PORT               GPIOB
#define ADS1299_BOARD_CS_GPIO_PIN                GPIO_PIN_6   /* Arduino D10 */
#define ADS1299_BOARD_DRDY_GPIO_PORT             GPIOC
#define ADS1299_BOARD_DRDY_GPIO_PIN              GPIO_PIN_7   /* Arduino D9  */
#define ADS1299_BOARD_RESET_GPIO_PORT            GPIOA
#define ADS1299_BOARD_RESET_GPIO_PIN             GPIO_PIN_9   /* Arduino D8  */
#define ADS1299_BOARD_PWDN_GPIO_PORT             GPIOA
#define ADS1299_BOARD_PWDN_GPIO_PIN              GPIO_PIN_8   /* Arduino D7  */
#define ADS1299_BOARD_START_GPIO_PORT            GPIOB
#define ADS1299_BOARD_START_GPIO_PIN             GPIO_PIN_10  /* Arduino D6  */

/*
 * Reference host transport: USART2 is connected to the ST-LINK VCP on the
 * default MB1136 solder-bridge configuration (PA2 TX, PA3 RX).  Do not expect
 * PA2/PA3 simultaneously on the Arduino/Morpho headers unless the documented
 * solder-bridge routing is changed.
 */
#define ADS1299_BOARD_STREAM_UART_INSTANCE       USART2
#define ADS1299_BOARD_STREAM_TX_GPIO_PORT        GPIOA
#define ADS1299_BOARD_STREAM_TX_GPIO_PIN         GPIO_PIN_2
#define ADS1299_BOARD_STREAM_RX_GPIO_PORT        GPIOA
#define ADS1299_BOARD_STREAM_RX_GPIO_PIN         GPIO_PIN_3
#define ADS1299_BOARD_STREAM_BAUD                921600u

/* ADS1299-8 and canonical repository packet invariants. */
#define ADS1299_BOARD_RAW_FRAME_BYTES            27u
#define ADS1299_BOARD_PACKET_SYNC0               0xA5u
#define ADS1299_BOARD_PACKET_SYNC1               0x5Au
#define ADS1299_BOARD_CANONICAL_PACKET_BYTES     49u

/* Beginner defaults. Shared core owns named-profile/register semantics. */
#define ADS1299_BOARD_BEGINNER_SAMPLE_RATE_SPS   250u
#define ADS1299_BOARD_BEGINNER_GAIN              24u

/*
 * Static bounded starter buffering.  For sustained acquisition, higher SPS,
 * or multiple ADS1299 devices, move acquisition/transport to DMA or IRQ-driven
 * bounded queues and count overflow explicitly.  Never block radio/host/printf
 * work in the DRDY timing path.
 */
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES        16u

#endif /* ADS1299_STM32F446_BOARD_CONFIG_H */
