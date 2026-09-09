#include <assert.h>
#include "ads1299_drdy.h"

static int drdy_level = 1;
static int fake_spi(void *u,const uint8_t*t,uint8_t*r,size_t n){(void)u;(void)t;(void)r;(void)n;return 0;}
static void fake_write(void *u,ads1299_platform_pin_t p,int v){(void)u;(void)p;(void)v;}
static int fake_read(void *u,ads1299_platform_pin_t p){(void)u; assert(p==ADS1299_PIN_DRDY); return drdy_level;}
static void fake_delay(void *u,uint32_t n){(void)u;(void)n;}

void test_port_drdy_active_low(void) {
    ads1299_platform_hal_t hal={0,fake_spi,fake_write,fake_read,fake_delay};
    ads1299_mcu_port_t ctx;
    assert(ads1299_mcu_port_init(&ctx,&hal)==0);
    ads1299_port_t port=ads1299_mcu_make_port(&ctx);

    drdy_level=1;
    assert(ads1299_mcu_data_ready(&ctx)==0);
    assert(port.drdy_read(port.user)==1);

    drdy_level=0;
    assert(ads1299_mcu_data_ready(&ctx)==1);
    assert(port.drdy_read(port.user)==0);
}
