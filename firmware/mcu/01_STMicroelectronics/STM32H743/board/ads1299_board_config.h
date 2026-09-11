#ifndef STM32H743_ADS1299_BOARD_CONFIG_H
#define STM32H743_ADS1299_BOARD_CONFIG_H

/*
 * STM32H743 ADS1299 board configuration
 *
 * Legacy reference hardware: ST NUCLEO-H743ZI / NUCLEO-H743ZI2 family.
 * ST currently marks the NUCLEO-H743ZI product obsolete, so this file is a
 * reproducible CubeH7 integration point rather than a claim that a new board
 * can still be purchased from ST. For another STM32H743 board, change only
 * this file and the CubeMX pin/peripheral setup; do not edit the shared
 * ADS1299 core.
 *
 * Required SPI settings in STM32CubeMX/STM32CubeIDE:
 *   master, full duplex, 8-bit, MSB first,
 *   CPOL=Low, CPHA=2 Edge (ADS1299 SPI Mode 1), software NSS.
 * ADS1299 CS must be an ordinary GPIO output.
 */

#include "main.h"

/* ---- SPI ---------------------------------------------------------------- */
/* Change this mapping only if the CubeMX project uses another SPI instance. */
extern SPI_HandleTypeDef hspi1;
#define ADS1299_BOARD_SPI_HANDLE (&hspi1)
#define ADS1299_BOARD_SPI_TIMEOUT_MS 100u

/* ---- ADS1299 control/data-ready GPIOs ----------------------------------- */
/*
 * Recommended CubeMX user labels:
 *   ADS1299_CS, ADS1299_DRDY, ADS1299_RESET, ADS1299_PWDN, ADS1299_START
 *
 * CubeMX then generates the *_GPIO_Port and *_Pin aliases consumed below.
 * Existing projects may map differently named aliases here instead.
 */
#ifndef ADS1299_CS_GPIO_Port
#error "Define/map ADS1299_CS_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_CS_Pin
#error "Define/map ADS1299_CS_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_DRDY_GPIO_Port
#error "Define/map ADS1299_DRDY_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_DRDY_Pin
#error "Define/map ADS1299_DRDY_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_RESET_GPIO_Port
#error "Define/map ADS1299_RESET_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_RESET_Pin
#error "Define/map ADS1299_RESET_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_PWDN_GPIO_Port
#error "Define/map ADS1299_PWDN_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_PWDN_Pin
#error "Define/map ADS1299_PWDN_Pin in ads1299_board_config.h"
#endif
#ifndef ADS1299_START_GPIO_Port
#error "Define/map ADS1299_START_GPIO_Port in ads1299_board_config.h"
#endif
#ifndef ADS1299_START_Pin
#error "Define/map ADS1299_START_Pin in ads1299_board_config.h"
#endif

#define ADS1299_BOARD_REFERENCE_NAME "NUCLEO-H743ZI/ZI2 (legacy)"

#endif /* STM32H743_ADS1299_BOARD_CONFIG_H */
