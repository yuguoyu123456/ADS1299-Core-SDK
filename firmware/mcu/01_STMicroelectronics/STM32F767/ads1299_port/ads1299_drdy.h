#ifndef ADS1299_MCU_DRDY_H
#define ADS1299_MCU_DRDY_H

#include "ads1299_spi.h"

int ads1299_port_drdy_read(void *user);
int ads1299_mcu_data_ready(ads1299_mcu_port_t *ctx);

#endif
