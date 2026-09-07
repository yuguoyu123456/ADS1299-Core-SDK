#include <assert.h>
#include "ads1299_spi.h"
#include "ads1299_gpio.h"

static ads1299_platform_pin_t last_pin; static int last_level;
static int fake_spi(void *u,const uint8_t*t,uint8_t*r,size_t n){(void)u;(void)t;(void)r;(void)n;return 0;}
static void fake_write(void *u, ads1299_platform_pin_t p, int v){(void)u;last_pin=p;last_level=v;}
static int fake_read(void *u, ads1299_platform_pin_t p){(void)u;(void)p;return 1;}
static uint32_t delayed_us;
static void fake_delay(void *u,uint32_t n){(void)u;delayed_us += n;}

void test_port_reset_line(void) {
    ads1299_platform_hal_t hal={0,fake_spi,fake_write,fake_read,fake_delay};
    ads1299_mcu_port_t ctx; assert(ads1299_mcu_port_init(&ctx,&hal)==0);
    ads1299_port_t port=ads1299_mcu_make_port(&ctx);
    port.reset_write(port.user,0);
    assert(last_pin==ADS1299_PIN_RESET && last_level==0);
    port.reset_write(port.user,1);
    assert(last_pin==ADS1299_PIN_RESET && last_level==1);
    ads1299_mcu_delay_us(&ctx, 25u);
    ads1299_mcu_delay_ms(&ctx, 2u);
    assert(delayed_us == 2025u);
}
