#ifndef RK3399_ADS1299_BOARD_CONFIG_H
#define RK3399_ADS1299_BOARD_CONFIG_H

#include <limits.h>
#include <stdint.h>

/* RK3399 Linux board configuration. Edit this file only for bring-up.
 * Resolve device nodes and GPIO line offsets from the exact carrier/BSP.
 * UINT_MAX means "not configured" so the example fails safely. */
#define RK3399_ADS1299_SPIDEV          "/dev/spidev0.0"
#define RK3399_ADS1299_GPIOCHIP        "/dev/gpiochip0"
#define RK3399_ADS1299_SPI_HZ          1000000u

#define RK3399_ADS1299_CS_LINE         UINT_MAX
#define RK3399_ADS1299_RESET_LINE      UINT_MAX
#define RK3399_ADS1299_PWDN_LINE       UINT_MAX
#define RK3399_ADS1299_START_LINE      UINT_MAX
#define RK3399_ADS1299_DRDY_LINE       UINT_MAX

#endif
