#include "esp_log.h"
extern int ads1299_example_main(void);
void app_main(void)
{
    int result=ads1299_example_main();
    ESP_LOGE("ads1299", "Example stopped with code %d; no automatic bus recovery",result);
}
