#include <stdint.h>

#include "ads1299_c6_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Original dedicated SPI2 reference. Avoid GPIO4/5 boot straps. */
static ads1299_c6_context_t context;
static TaskHandle_t drdy_waiter;

static void IRAM_ATTR board_ads1299_drdy_isr(void *arg)
{
    (void)arg;
    BaseType_t higher_priority_woken = pdFALSE;
    TaskHandle_t waiter = drdy_waiter;
    if (waiter != NULL) {
        vTaskNotifyGiveFromISR(waiter, &higher_priority_woken);
        if (higher_priority_woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
}

void board_ads1299_idle(void) { vTaskDelay(1); }

/* Blocking DRDY wait for the sustained-acquisition task.  The level checks
 * before and after publishing the waiter close the common edge-before-wait
 * race.  The shared core still owns ADS1299 timing and frame semantics. */
int board_ads1299_wait_drdy(uint32_t timeout_ms)
{
    if (gpio_get_level(GPIO_NUM_3) == 0) return 0;

    (void)ulTaskNotifyTake(pdTRUE, 0);
    drdy_waiter = xTaskGetCurrentTaskHandle();

    if (gpio_get_level(GPIO_NUM_3) == 0) {
        drdy_waiter = NULL;
        return 0;
    }

    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    if (timeout_ticks == 0) timeout_ticks = 1;
    uint32_t notified = ulTaskNotifyTake(pdTRUE, timeout_ticks);
    drdy_waiter = NULL;
    return notified != 0u ? 0 : -1;
}

int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    const gpio_num_t pins[5]={GPIO_NUM_10,GPIO_NUM_18,GPIO_NUM_19,GPIO_NUM_1,GPIO_NUM_3};
    gpio_config_t out={.pin_bit_mask=(1ULL<<10)|(1ULL<<18)|(1ULL<<19)|(1ULL<<1),
        .mode=GPIO_MODE_OUTPUT,.pull_up_en=GPIO_PULLUP_DISABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,.intr_type=GPIO_INTR_DISABLE};
    gpio_config_t in={.pin_bit_mask=1ULL<<3,.mode=GPIO_MODE_INPUT,
        .pull_up_en=GPIO_PULLUP_ENABLE,.pull_down_en=GPIO_PULLDOWN_DISABLE,
        .intr_type=GPIO_INTR_NEGEDGE};
    /* Preload safe levels before enabling output drivers. */
    for(unsigned i=0;i<4;++i)
        if (gpio_set_level(pins[i],i==3 ? 0 : 1)!=ESP_OK) return -1;
    if (gpio_config(&out)!=ESP_OK || gpio_config(&in)!=ESP_OK) return -1;

    esp_err_t isr_rc = gpio_install_isr_service(0);
    if (isr_rc != ESP_OK && isr_rc != ESP_ERR_INVALID_STATE) return -1;
    if (gpio_isr_handler_add(GPIO_NUM_3, board_ads1299_drdy_isr, NULL) != ESP_OK)
        return -1;

    const spi_bus_config_t bus={.mosi_io_num=7,.miso_io_num=2,.sclk_io_num=6,
        .quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=32};
    const spi_device_interface_config_t dev={.clock_speed_hz=1000000,.mode=1,
        .spics_io_num=-1,.queue_size=1};
    if (spi_bus_initialize(SPI2_HOST,&bus,SPI_DMA_DISABLED)!=ESP_OK) return -1;
    if (spi_bus_add_device(SPI2_HOST,&dev,&context.spi)!=ESP_OK) {
        (void)spi_bus_free(SPI2_HOST); return -1;
    }
    for(unsigned i=0;i<5;++i) context.pins[i]=pins[i];
    context.timeout_ticks=pdMS_TO_TICKS(100);
    return ads1299_c6_make_hal(&context,hal);
}
