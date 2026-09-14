#ifndef RK3288_ADS1299_BOARD_CONFIG_H
#define RK3288_ADS1299_BOARD_CONFIG_H

#include <limits.h>
#include <stdint.h>

/*
 * RK3288 Linux board configuration.
 *
 * Edit this file only for first bring-up. Device nodes and GPIO line offsets
 * are Linux userspace identifiers, not SoC bank/pin numbers. Resolve them from
 * the exact board/BSP device tree and `gpioinfo` output before running.
 *
 * UINT_MAX intentionally means "not configured" so the example fails loudly
 * instead of driving an arbitrary GPIO on an unknown RK3288 carrier board.
 */
#define RK3288_ADS1299_SPIDEV          "/dev/spidev0.0"
#define RK3288_ADS1299_GPIOCHIP        "/dev/gpiochip0"
#define RK3288_ADS1299_SPI_HZ          1000000u

#define RK3288_ADS1299_CS_LINE         UINT_MAX
#define RK3288_ADS1299_RESET_LINE      UINT_MAX
#define RK3288_ADS1299_PWDN_LINE       UINT_MAX
#define RK3288_ADS1299_START_LINE      UINT_MAX
#define RK3288_ADS1299_DRDY_LINE       UINT_MAX

#endif
