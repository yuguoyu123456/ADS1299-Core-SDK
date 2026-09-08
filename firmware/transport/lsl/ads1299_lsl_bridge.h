#ifndef ADS1299_LSL_BRIDGE_H
#define ADS1299_LSL_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float channels_uv[ADS1299_PACKET_CHANNELS];
    uint32_t sequence;
    uint32_t device_timestamp_us;
    uint8_t flags;
    uint8_t status[ADS1299_PACKET_STATUS_BYTES];
} ads1299_lsl_sample_t;

/* Decode one canonical ADS1299 packet and convert ADC codes to input-referred
 * microvolts for a host-side LSL bridge. Returns 0 on success.
 */
int ads1299_lsl_packet_to_sample(const uint8_t *packet,
                                 size_t packet_length,
                                 double vref_volts,
                                 unsigned gain,
                                 ads1299_lsl_sample_t *out);

/* Copy only the channel values into an LSL-friendly float array. */
void ads1299_lsl_copy_channels(const ads1299_lsl_sample_t *sample,
                               float out_channels[ADS1299_PACKET_CHANNELS]);

#ifdef __cplusplus
}
#endif

#endif
