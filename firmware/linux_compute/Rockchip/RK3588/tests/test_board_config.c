#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../board_config.h"

static int failures;

static void check(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

int main(void)
{
    check(RK3588_ADS1299_SPIDEV[0] == '/', "spidev path must be absolute");
    check(strncmp(RK3588_ADS1299_SPIDEV, "/dev/spidev", 11) == 0,
          "spidev path must use the Linux spidev device family");
    check(RK3588_ADS1299_GPIOCHIP[0] == '/', "gpiochip path must be absolute");
    check(strncmp(RK3588_ADS1299_GPIOCHIP, "/dev/gpiochip", 13) == 0,
          "GPIO path must use the Linux GPIO character-device family");
    check(RK3588_ADS1299_SPI_HZ > 0u, "SPI clock must be non-zero");

    /* The template intentionally leaves board-specific lines unconfigured.
     * This protects new users from silently driving guessed RK3588 pins. */
    check(RK3588_ADS1299_CS_LINE == UINT_MAX, "template CS must fail safe");
    check(RK3588_ADS1299_RESET_LINE == UINT_MAX, "template RESET must fail safe");
    check(RK3588_ADS1299_PWDN_LINE == UINT_MAX, "template PWDN must fail safe");
    check(RK3588_ADS1299_START_LINE == UINT_MAX, "template START must fail safe");
    check(RK3588_ADS1299_DRDY_LINE == UINT_MAX, "template DRDY must fail safe");

    if (failures != 0) {
        fprintf(stderr, "RK3588 board-config contract: %d failure(s)\n", failures);
        return 1;
    }

    puts("RK3588 board-config contract: PASS");
    return 0;
}
