#ifndef ADS1299_MCU_GPIO_H
#define ADS1299_MCU_GPIO_H

#include "ads1299_spi.h"

void ads1299_port_cs_write(void *user, int level);
void ads1299_port_reset_write(void *user, int level);
void ads1299_port_pwdn_write(void *user, int level);
void ads1299_port_start_write(void *user, int level);
void ads1299_mcu_cs_low(ads1299_mcu_port_t *ctx);
void ads1299_mcu_cs_high(ads1299_mcu_port_t *ctx);
void ads1299_mcu_reset(ads1299_mcu_port_t *ctx, int level);
void ads1299_mcu_start(ads1299_mcu_port_t *ctx, int level);
void ads1299_mcu_delay_us(ads1299_mcu_port_t *ctx, uint32_t us);
void ads1299_mcu_delay_ms(ads1299_mcu_port_t *ctx, uint32_t ms);

#endif
