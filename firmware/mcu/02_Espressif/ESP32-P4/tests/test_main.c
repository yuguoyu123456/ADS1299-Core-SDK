#include <stdio.h>

void test_frame_queue_fifo_wraparound_and_overflow(void);
void test_frame_queue_argument_validation(void);
void test_frame_queue_long_running_interleaving(void);

int main(void)
{
    test_frame_queue_fifo_wraparound_and_overflow();
    test_frame_queue_argument_validation();
    test_frame_queue_long_running_interleaving();
    puts("ESP32-P4 bounded frame queue tests: PASS");
    return 0;
}
