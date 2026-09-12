#include <stdio.h>

void esp32c6_test_frame_queue(void);

int main(void)
{
    esp32c6_test_frame_queue();
    puts("ESP32-C6 bounded frame queue tests: PASS");
    return 0;
}
