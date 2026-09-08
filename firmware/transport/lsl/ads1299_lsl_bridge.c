#include "ads1299_lsl_bridge.h"

#include <string.h>

int ads1299_lsl_packet_to_sample(const uint8_t *packet,
                                 size_t packet_length,
                                 double vref_volts,
                                 unsigned gain,
                                 ads1299_lsl_sample_t *out)
{
    ads1299_packet_t decoded;
    size_t i;

    if (!packet || !out) return -1;
    if (vref_volts <= 0.0 || gain == 0u) return -2;
    if (ads1299_packet_decode(packet, packet_length, &decoded) != ADS1299_PACKET_OK)
        return -3;

    memset(out, 0, sizeof(*out));
    out->sequence = decoded.sequence;
    out->device_timestamp_us = decoded.timestamp_us;
    out->flags = decoded.flags;
    memcpy(out->status, decoded.status, ADS1299_PACKET_STATUS_BYTES);

    for (i = 0u; i < ADS1299_PACKET_CHANNELS; ++i) {
        out->channels_uv[i] = (float)ads1299_code_to_microvolts(decoded.channels[i],
                                                                vref_volts,
                                                                gain);
    }

    return 0;
}

void ads1299_lsl_copy_channels(const ads1299_lsl_sample_t *sample,
                               float out_channels[ADS1299_PACKET_CHANNELS])
{
    if (!sample || !out_channels) return;
    memcpy(out_channels,
           sample->channels_uv,
           sizeof(sample->channels_uv));
}
