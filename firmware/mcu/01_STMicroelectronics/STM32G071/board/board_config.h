#ifndef ADS1299_STM32G071_BOARD_CONFIG_H
#define ADS1299_STM32G071_BOARD_CONFIG_H

/*
 * ADS1299-Core-SDK STM32G071 reference-board configuration
 *
 * Reference board : NUCLEO-G071RB (MB1360)
 * MCU             : STM32G071RBT6
 * Toolchain       : STM32CubeMX / STM32CubeIDE + STM32CubeG0 HAL
 *
 * This is the single obvious repository-owned hardware configuration point
 * for the beginner path. When moving to another STM32G071 board, change the
 * CubeMX routing and this board layer; do not edit shared ADS1299 register,
 * model, frame, profile, or packet logic.
 */

/* ADS1299 SPI contract: Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first. */
#define ADS1299_BOARD_SPI_INSTANCE              SPI1
#define ADS1299_BOARD_SPI_CPOL                  0u
#define ADS1299_BOARD_SPI_CPHA                  1u
#define ADS1299_BOARD_SPI_MSB_FIRST             1u

/*
 * NUCLEO-G071RB / MB1360 Arduino routing from ST UM2324:
 * D13=PA5/SPI1_SCK, D12=PA6/SPI1_MISO, D11=PA7/SPI1_MOSI.
 * PA5 also drives the board USER LED; this is a reference bring-up mapping.
 */
#define ADS1299_BOARD_SCK_GPIO_PORT             GPIOA
#define ADS1299_BOARD_SCK_GPIO_PIN              GPIO_PIN_5
#define ADS1299_BOARD_MISO_GPIO_PORT            GPIOA
#define ADS1299_BOARD_MISO_GPIO_PIN             GPIO_PIN_6
#define ADS1299_BOARD_MOSI_GPIO_PORT            GPIOA
#define ADS1299_BOARD_MOSI_GPIO_PIN             GPIO_PIN_7

/*
 * Starter control routing uses header-accessible GPIOs. Keep CS under GPIO
 * control so shared-SPI + independent-CS multi-ADS1299 designs remain possible.
 */
#define ADS1299_BOARD_CS_GPIO_PORT              GPIOB
#define ADS1299_BOARD_CS_GPIO_PIN               GPIO_PIN_0   /* Arduino D10 */
#define ADS1299_BOARD_DRDY_GPIO_PORT            GPIOC
#define ADS1299_BOARD_DRDY_GPIO_PIN             GPIO_PIN_7   /* Arduino D9  */
#define ADS1299_BOARD_RESET_GPIO_PORT           GPIOA
#define ADS1299_BOARD_RESET_GPIO_PIN            GPIO_PIN_9   /* Arduino D8  */
#define ADS1299_BOARD_PWDN_GPIO_PORT            GPIOA
#define ADS1299_BOARD_PWDN_GPIO_PIN             GPIO_PIN_8   /* Arduino D7  */
#define ADS1299_BOARD_START_GPIO_PORT           GPIOB
#define ADS1299_BOARD_START_GPIO_PIN            GPIO_PIN_14  /* Arduino D6  */

/*
 * Reference host transport: UART2 is connected to the ST-LINK VCP on the
 * default MB1360 configuration (PA2 TX, PA3 RX). Keep console/stream work out
 * of the DRDY timing path.
 */
#define ADS1299_BOARD_STREAM_UART_INSTANCE      USART2
#define ADS1299_BOARD_STREAM_TX_GPIO_PORT       GPIOA
#define ADS1299_BOARD_STREAM_TX_GPIO_PIN        GPIO_PIN_2
#define ADS1299_BOARD_STREAM_RX_GPIO_PORT       GPIOA
#define ADS1299_BOARD_STREAM_RX_GPIO_PIN        GPIO_PIN_3
#define ADS1299_BOARD_STREAM_BAUD               460800u

/* ADS1299-8 and canonical repository packet invariants. */
#define ADS1299_BOARD_RAW_FRAME_BYTES           27u
#define ADS1299_BOARD_PACKET_SYNC0              0xA5u
#define ADS1299_BOARD_PACKET_SYNC1              0x5Au
#define ADS1299_BOARD_CANONICAL_PACKET_BYTES    49u

/* Beginner defaults. Shared core owns named-profile/register semantics. */
#define ADS1299_BOARD_BEGINNER_SAMPLE_RATE_SPS  250u
#define ADS1299_BOARD_BEGINNER_GAIN             24u
#define ADS1299_BOARD_DRDY_TIMEOUT_MS           20u

/*
 * Static bounded buffering is intentional for this small MCU class. Avoid heap
 * allocation and floating-point conversion in acquisition timing paths.
 */
#define ADS1299_BOARD_STREAM_QUEUE_FRAMES       8u

/* 49 bytes * 250 SPS = 12250 bytes/s; 460800 8N1 has adequate starter margin. */
#define ADS1299_BOARD_STREAM_REQUIRED_BPS       (ADS1299_BOARD_CANONICAL_PACKET_BYTES * ADS1299_BOARD_BEGINNER_SAMPLE_RATE_SPS * 10u)

#endif /* ADS1299_STM32G071_BOARD_CONFIG_H */
