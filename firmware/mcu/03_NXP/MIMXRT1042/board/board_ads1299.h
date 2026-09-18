#ifndef MIMXRT1042_BOARD_ADS1299_H
#define MIMXRT1042_BOARD_ADS1299_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Single board-integration seam for all MIMXRT1042 beginner examples.
 *
 * The reference target is the MCUXpresso evkmimxrt1040 application. Keep pin
 * mux, LPSPI instance/source clock, ADS1299 GPIO pins, delay source and output
 * transport in the board/application layer. Moving to another MIMXRT1042 board
 * must not require edits to the shared ADS1299 core or to beginner examples.
 *
 * Return 0 only after the board has initialized its pins/peripheral and filled
 * every required ads1299_platform_hal_t callback. A non-zero result is a
 * board/port configuration failure, intentionally separate from SPI/probe/ID
 * failures reported by the examples.
 */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

#endif
