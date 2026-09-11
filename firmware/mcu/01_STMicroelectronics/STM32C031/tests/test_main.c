#include <stdio.h>

void test_core_frame_numeric_boundary(void);
void test_register_boundary_is_opaque(void);
void test_port_reset_line(void);
void test_port_spi(void);
void test_port_drdy_active_low(void);
void test_canonical_packet_round_trip(void);

int main(void) {
    puts("STM32C031 ADS1299 integration smoke tests");

    test_core_frame_numeric_boundary();
    puts("  PASS frame numeric boundaries");

    test_register_boundary_is_opaque();
    puts("  PASS register boundary remains in shared core");

    test_port_spi();
    puts("  PASS SPI port transport");

    test_port_reset_line();
    puts("  PASS RESET/delay routing");

    test_port_drdy_active_low();
    puts("  PASS DRDY active-low routing");

    test_canonical_packet_round_trip();
    puts("  PASS canonical packet round-trip and CRC detection");

    puts("PASS STM32C031 integration smoke tests");
    return 0;
}
