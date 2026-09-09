#ifndef ADS1299_STM32F429_BOARD_CONFIG_H
#define ADS1299_STM32F429_BOARD_CONFIG_H

/*
 * ADS1299-Core-SDK STM32F429 reference-board configuration
 *
 * Reference board : STM32F429I-DISC1 (MB1075)
 * MCU             : STM32F429ZIT6
 * Toolchain       : STM32CubeMX / STM32CubeIDE + STM32CubeF4 HAL
 *
 * This file is intentionally the obvious board-dependent edit point for a
 * beginner.  Shared ADS1299 register/model/frame logic must not be changed
 * when moving to another STM32F429 board.
 *
 * IMPORTANT: verify the selected GPIOs against the exact MB1075 revision and
 * the current CubeMX pinout before wiring.  The Discovery board contains LCD,
 * SDRAM, MEMS and ST-LINK/VCP circuitry, so alternate-function conflicts must
 * be resolved in the board layer rather than in the ADS1299 core.
 */

/* ADS1299 SPI contract: Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first. */
#define ADS1299_BOARD_SPI_INSTANCE              SPI1
#define ADS1299_BOARD_SPI_CPOL                  0u
#define ADS1299_BOARD_SPI_CPHA                  1u
#define ADS1299_BOARD_SPI_MSB_FIRST             1u

/* Starter SPI1 routing. Configure the same alternate functions in CubeMX. */
#define ADS1299_BOARD_SCK_GPIO_PORT              GPIOA
#define ADS1299_BOARD_SCK_GPIO_PIN               GPIO_PIN_5
#define ADS1299_BOARD_MISO_GPIO_PORT             GPIOA
#define ADS1299_BOARD_MISO_GPIO_PIN              GPIO_PIN_6
#define ADS1299_BOARD_MOSI_GPIO_PORT             GPIOA
#define ADS1299_BOARD_MOSI_GPIO_PIN              GPIO_PIN_7

/* Software-controlled ADS1299 control pins. */
#define ADS1299_BOARD_CS_GPIO_PORT               GPIOB
#define ADS1299_BOARD_CS_GPIO_PIN                GPIO_PIN_0
#define ADS1299_BOARD_DRDY_GPIO_PORT             GPIOB
#define ADS1299_BOARD_DRDY_GPIO_PIN              GPIO_PIN_1
#define ADS1299_BOARD_RESET_GPIO_PORT            GPIOB
#define ADS1299_BOARD_RESET_GPIO_PIN             GPIO_PIN_11
#define ADS1299_BOARD_PWDN_GPIO_PORT             GPIOB
#define ADS1299_BOARD_PWDN_GPIO_PIN              GPIO_PIN_12
#define ADS1299_BOARD_START_GPIO_PORT            GPIOB
#define ADS1299_BOARD_START_GPIO_PIN             GPIO_PIN_13

/*
 * Reference host transport: ST-LINK/V2-B VCP on STM32F429I-DISC1 uses
 * USART1.  PA9 is TX and PA10 is RX when the relevant solder bridges are
 * connected.  Only TX is required for the starter packet stream.
 */
#define ADS1299_BOARD_STREAM_UART_INSTANCE       USART1
#define ADS1299_BOARD_STREAM_TX_GPIO_PORT        GPIOA
#define ADS1299_BOARD_STREAM_TX_GPIO_PIN         GPIO_PIN_9
#define ADS1299_BOARD_STREAM_RX_GPIO_PORT        GPIOA
#define ADS1299_BOARD_STREAM_RX_GPIO_PIN         GPIO_PIN_10
#define ADS1299_BOARD_STREAM_BAUD                921600u

/* ADS1299-8 and repository packet invariants. */
#define ADS1299_BOARD_RAW_FRAME_BYTES            27u
#define ADS1299_BOARD_PACKET_SYNC0               0xA5u
#define ADS1299_BOARD_PACKET_SYNC1               0x5Au
#define ADS1299_BOARD_CANONICAL_PACKET_BYTES     49u

/*
 * Static bounded starter queue.  F429-class sustained or multi-device
 * acquisition should move SPI/transport work to DMA/ISR + bounded buffers and
 * must account for overflow explicitly instead of blocking the DRDY path.
 */
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES        16u

/* Beginner defaults. Shared core owns the actual named profile semantics. */
#define ADS1299_BOARD_BEGINNER_SAMPLE_RATE_SPS   250u
#define ADS1299_BOARD_BEGINNER_GAIN              24u

#endif /* ADS1299_STM32F429_BOARD_CONFIG_H */
