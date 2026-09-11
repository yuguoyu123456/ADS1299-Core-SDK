#include <stdio.h>

void test_core_frame_numeric_boundary(void);
void test_register_boundary_is_opaque(void);
void test_port_spi(void);
void test_port_reset_line(void);
void test_port_drdy_active_low(void);
void test_canonical_packet_round_trip(void);
void test_frame_queue_fifo_wraparound_and_overflow(void);
void test_frame_queue_argument_validation(void);
void test_frame_queue_long_running_interleaving(void);

int main(void) {
    test_core_frame_numeric_boundary();
    test_register_boundary_is_opaque();
    test_port_spi();
    test_port_reset_line();
    test_port_drdy_active_low();
    test_canonical_packet_round_trip();
    test_frame_queue_fifo_wraparound_and_overflow();
    test_frame_queue_argument_validation();
    test_frame_queue_long_running_interleaving();
    puts("PASS STM32H723 integration smoke tests");
    return 0;
}
