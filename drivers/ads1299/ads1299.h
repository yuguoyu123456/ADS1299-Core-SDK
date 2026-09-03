#ifndef ADS1299_H
#define ADS1299_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_port.h"
#include "ads1299_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADS1299_OK = 0,
    ADS1299_EINVAL = -1,
    ADS1299_EIO = -2,
    ADS1299_ETIMEOUT = -3
} ads1299_status_t;

typedef struct {
    uint8_t status[ADS1299_STATUS_BYTES];
    int32_t channel[ADS1299_CHANNEL_COUNT];
} ads1299_frame_t;

typedef struct {
    ads1299_port_t port;
    uint8_t continuous_mode;
} ads1299_t;

ads1299_status_t ads1299_init(ads1299_t *dev, const ads1299_port_t *port);
ads1299_status_t ads1299_command(ads1299_t *dev, uint8_t command);
ads1299_status_t ads1299_hardware_reset(ads1299_t *dev);
ads1299_status_t ads1299_reset_command(ads1299_t *dev);
ads1299_status_t ads1299_start(ads1299_t *dev);
ads1299_status_t ads1299_stop(ads1299_t *dev);
ads1299_status_t ads1299_rdatac(ads1299_t *dev);
ads1299_status_t ads1299_sdatac(ads1299_t *dev);

ads1299_status_t ads1299_read_register(ads1299_t *dev, uint8_t address, uint8_t *value);
ads1299_status_t ads1299_write_register(ads1299_t *dev, uint8_t address, uint8_t value);
ads1299_status_t ads1299_read_registers(ads1299_t *dev, uint8_t address, uint8_t *values, size_t count);
ads1299_status_t ads1299_write_registers(ads1299_t *dev, uint8_t address, const uint8_t *values, size_t count);

ads1299_status_t ads1299_set_data_rate(ads1299_t *dev, uint8_t dr_code);
ads1299_status_t ads1299_set_daisy_chain_mode(ads1299_t *dev, int enable);
ads1299_status_t ads1299_set_channel(ads1299_t *dev, uint8_t channel_1_to_8,
                                    uint8_t gain_code, uint8_t mux_code,
                                    int srb2, int power_down);
uint8_t ads1299_make_internal_test_config2(int amplitude_x2, uint8_t freq_code);
ads1299_status_t ads1299_configure_internal_test(ads1299_t *dev,
                                                 uint8_t gain_code,
                                                 int amplitude_x2,
                                                 uint8_t freq_code);
ads1299_status_t ads1299_configure_input_short_test(ads1299_t *dev,
                                                    uint8_t gain_code);
ads1299_status_t ads1299_read_frame_continuous(ads1299_t *dev, ads1299_frame_t *frame);
ads1299_status_t ads1299_read_frame_rdata(ads1299_t *dev, ads1299_frame_t *frame);

int32_t ads1299_sign_extend24(uint32_t value24);
double ads1299_code_to_volts(int32_t code, double vref_volts, double gain);

#ifdef __cplusplus
}
#endif

#endif
