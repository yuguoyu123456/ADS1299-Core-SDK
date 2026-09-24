/*
 * Host smoke-test runner for the CC1312R7 ADS1299 port layer.
 *
 * Build this runner together with:
 *   test_spi.c test_reset.c test_drdy.c
 *   ../ads1299_port/ads1299_spi.c
 *   ../ads1299_port/ads1299_gpio.c
 *   ../ads1299_port/ads1299_drdy.c
 *
 * This is a host/software test only; it does not imply TI SDK or board validation.
 */
#include <stdio.h>

void test_port_spi(void);
void test_port_reset_line(void);
void test_port_drdy(void);

int main(void)
{
    test_port_spi();
    test_port_reset_line();
    test_port_drdy();

    puts("CC1312R7 ADS1299 port smoke tests: PASS");
    return 0;
}
