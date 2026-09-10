#ifndef ADS1299_STM32G4_BOARD_CONFIG_H
#define ADS1299_STM32G4_BOARD_CONFIG_H

/*
 * Reference configuration for NUCLEO-G474RE (MB1367 / STM32G474RET6).
 *
 * This is the single repository-owned beginner configuration point for the
 * STM32G4 model folder. Keep ADS1299 protocol/register behavior in the shared
 * core. A custom STM32G4 board should normally change only values in this file
 * plus its CubeMX-generated peripheral initialization.
 *
 * Validation status: TEMPLATE / integration configuration present.
 * No target-build or board-hardware verification is claimed here.
 */

/* Reference board identity. */
#define ADS1299_G4_REFERENCE_BOARD_NAME      "NUCLEO-G474RE"
#define ADS1299_G4_REFERENCE_BOARD_MB        "MB1367"
#define ADS1299_G4_REFERENCE_MCU             "STM32G474RET6"

/* ADS1299 SPI contract. */
#define ADS1299_G4_SPI_INSTANCE              SPI1
#define ADS1299_G4_SPI_CPOL                  0u
#define ADS1299_G4_SPI_CPHA                  1u
#define ADS1299_G4_SPI_MODE                  1u
#define ADS1299_G4_SPI_BITS                  8u
#define ADS1299_G4_SPI_MSB_FIRST             1u

/*
 * NUCLEO-G474RE Arduino Uno V3 header mapping from UM2505:
 * D13=PA5 SPI1_SCK, D12=PA6 SPI1_MISO, D11=PA7 SPI1_MOSI,
 * D10=PB6, D9=PC7, D8=PA9, D7=PA8, D6=PB10.
 */
#define ADS1299_G4_SCK_PORT                  GPIOA
#define ADS1299_G4_SCK_PIN                   GPIO_PIN_5
#define ADS1299_G4_MISO_PORT                 GPIOA
#define ADS1299_G4_MISO_PIN                  GPIO_PIN_6
#define ADS1299_G4_MOSI_PORT                 GPIOA
#define ADS1299_G4_MOSI_PIN                  GPIO_PIN_7

#define ADS1299_G4_CS_PORT                   GPIOB
#define ADS1299_G4_CS_PIN                    GPIO_PIN_6
#define ADS1299_G4_DRDY_PORT                 GPIOC
#define ADS1299_G4_DRDY_PIN                  GPIO_PIN_7
#define ADS1299_G4_RESET_PORT                GPIOA
#define ADS1299_G4_RESET_PIN                 GPIO_PIN_9
#define ADS1299_G4_PWDN_PORT                 GPIOA
#define ADS1299_G4_PWDN_PIN                  GPIO_PIN_8
#define ADS1299_G4_START_PORT                GPIOB
#define ADS1299_G4_START_PIN                 GPIO_PIN_10

/* Starter acquisition/profile constants. */
#define ADS1299_G4_SAMPLE_RATE_SPS           250u
#define ADS1299_G4_DEFAULT_GAIN              24u
#define ADS1299_G4_RAW_FRAME_BYTES           27u
#define ADS1299_G4_CANONICAL_PACKET_BYTES    49u
#define ADS1299_G4_DRDY_TIMEOUT_MS           20u

/*
 * Host transport for the reference board.
 * UM2505 documents LPUART1 on PA2/PA3 connected to STLINK-V3E VCP in the
 * default solder-bridge configuration. 460800 baud leaves useful margin for
 * the canonical 49-byte x 250-SPS stream (12,250 payload bytes/s).
 */
#define ADS1299_G4_STREAM_UART_INSTANCE      LPUART1
#define ADS1299_G4_STREAM_TX_PORT            GPIOA
#define ADS1299_G4_STREAM_TX_PIN             GPIO_PIN_2
#define ADS1299_G4_STREAM_RX_PORT            GPIOA
#define ADS1299_G4_STREAM_RX_PIN             GPIO_PIN_3
#define ADS1299_G4_STREAM_BAUD               460800u
#define ADS1299_G4_STREAM_MIN_BAUD_8N1       122500u

/* Bounded starter buffering. Scale only after measuring RAM and overflow. */
#define ADS1299_G4_FRAME_QUEUE_DEPTH         16u

#endif /* ADS1299_STM32G4_BOARD_CONFIG_H */
