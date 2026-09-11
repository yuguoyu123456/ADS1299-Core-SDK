#include <stdio.h>

void test_core_frame_numeric_boundary(void);
void test_register_boundary_is_opaque(void);
void test_port_reset_line(void);
void test_port_spi(void);
void test_port_drdy_active_low(void);
void test_canonical_packet_round_trip(void);

static int run_test(const char *name, void (*fn)(void)) {
    printf("[ RUN      ] %s\n", name);
    fn();
    printf("[       OK ] %s\n", name);
    return 0;
}

int main(void) {
    int failures = 0;

    failures += run_test("core_frame_numeric_boundary", test_core_frame_numeric_boundary);
    failures += run_test("register_boundary_is_opaque", test_register_boundary_is_opaque);
    failures += run_test("port_spi", test_port_spi);
    failures += run_test("port_reset_line", test_port_reset_line);
    failures += run_test("port_drdy_active_low", test_port_drdy_active_low);
    failures += run_test("canonical_packet_round_trip", test_canonical_packet_round_trip);

    if (failures != 0) {
        printf("FAIL STM32F407 integration smoke tests (%d failures)\n", failures);
        return 1;
    }

    printf("PASS STM32F407 integration smoke tests\n");
    return 0;
}
