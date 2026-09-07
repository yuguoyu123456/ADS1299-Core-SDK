#include "ads1299_esp32_hal.h"
#include "freertos/task.h"
/* Original DevKitC V4 WROOM profile; SPI3 is VSPI on classic ESP32. */
static ads1299_esp32_context_t context;
void board_ads1299_idle(void) { vTaskDelay(1); }
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    const gpio_num_t pins[5]={GPIO_NUM_21,GPIO_NUM_22,GPIO_NUM_25,GPIO_NUM_26,GPIO_NUM_27};
    gpio_config_t out={.pin_bit_mask=(1ULL<<21)|(1ULL<<22)|(1ULL<<25)|(1ULL<<26),
        .mode=GPIO_MODE_OUTPUT,.pull_up_en=GPIO_PULLUP_DISABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,.intr_type=GPIO_INTR_DISABLE};
    gpio_config_t in={.pin_bit_mask=1ULL<<27,.mode=GPIO_MODE_INPUT,
        .pull_up_en=GPIO_PULLUP_ENABLE,.pull_down_en=GPIO_PULLDOWN_DISABLE,
        .intr_type=GPIO_INTR_DISABLE};
    for(unsigned i=0;i<4;++i)
        if (gpio_set_level(pins[i],i==3 ? 0 : 1)!=ESP_OK) return -1;
    if (gpio_config(&out)!=ESP_OK || gpio_config(&in)!=ESP_OK) return -1;
    const spi_bus_config_t bus={.mosi_io_num=23,.miso_io_num=19,.sclk_io_num=18,
        .quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=32};
    const spi_device_interface_config_t dev={.clock_speed_hz=1000000,.mode=1,
        .spics_io_num=-1,.queue_size=1};
    if (spi_bus_initialize(SPI3_HOST,&bus,SPI_DMA_DISABLED)!=ESP_OK) return -1;
    if (spi_bus_add_device(SPI3_HOST,&dev,&context.spi)!=ESP_OK) {
        (void)spi_bus_free(SPI3_HOST); return -1;
    }
    for(unsigned i=0;i<5;++i) context.pins[i]=pins[i];
    context.timeout_ticks=pdMS_TO_TICKS(100);
    return ads1299_esp32_make_hal(&context,hal);
}
