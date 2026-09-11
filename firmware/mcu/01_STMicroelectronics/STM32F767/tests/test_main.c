#include <stdio.h>

void test_core_frame_numeric_boundary(void);
void test_register_boundary_is_opaque(void);
void test_port_spi(void);
void test_port_reset_line(void);
void test_port_drdy_active_low(void);
void test_canonical_packet_round_trip(void);

int main(void) {
    test_core_frame_numeric_boundary();
    test_register_boundary_is_opaque();
    test_port_spi();
    test_port_reset_line();
    test_port_drdy_active_low();
    test_canonical_packet_round_trip();
    puts("PASS STM32F767 integration smoke tests");
    return 0;
}
