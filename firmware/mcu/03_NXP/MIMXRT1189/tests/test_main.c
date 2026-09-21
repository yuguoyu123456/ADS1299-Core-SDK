#include <stdio.h>

void test_port_spi(void);
void test_port_reset_line(void);
void test_core_frame_numeric_boundary(void);
void test_register_constants(void);
void test_control_gpio_routing(void);
void test_delay_routing(void);

int main(void)
{
    test_port_spi();
    test_port_reset_line();
    test_core_frame_numeric_boundary();
    test_register_constants();
    test_control_gpio_routing();
    test_delay_routing();

    puts("MIMXRT1189 ADS1299 platform smoke tests: PASS");
    return 0;
}
