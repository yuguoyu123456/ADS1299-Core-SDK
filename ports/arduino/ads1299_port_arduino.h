#ifndef ADS1299_PORT_ARDUINO_H
#define ADS1299_PORT_ARDUINO_H

#include <Arduino.h>
#include <SPI.h>
extern "C" {
#include "ads1299_port.h"
}

typedef struct {
    SPIClass *spi;
    int cs_pin;
    int reset_pin;
    int start_pin;
    int drdy_pin;
    uint32_t spi_clock_hz;
} ads1299_arduino_ctx_t;

ads1299_port_t ads1299_arduino_make_port(ads1299_arduino_ctx_t *ctx);

#endif
