#ifndef MIMXRT1021_BOARD_ADS1299_H
#define MIMXRT1021_BOARD_ADS1299_H

#include "../ads1299_port/ads1299_spi.h"

/*
 * Single board-integration seam for all MIMXRT1021 beginner examples.
 *
 * Implement board_ads1299_hal() in board_ads1299.c inside the MCUXpresso
 * evkmimxrt1020 application. Keep pin mux, LPSPI instance/clock, GPIO pins,
 * delay source and transport choices in that board file; do not edit the
 * shared ADS1299 core or the examples when moving to another MIMXRT1021 board.
 *
 * Return 0 only after the board has initialized the pins/peripheral and filled
 * every required ads1299_platform_hal_t callback. Non-zero is a board/port
 * configuration failure and is intentionally distinct from ADS1299 probe/ID
 * failures reported by the examples.
 */
int board_ads1299_hal(ads1299_platform_hal_t *hal);

#endif
